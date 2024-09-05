function makeRequest(url, options) {
    return new Promise((resolve, reject) => {
        const xhr = new XMLHttpRequest();

        xhr.onreadystatechange = function () {
            if (xhr.readyState === XMLHttpRequest.DONE) {
                if (xhr.status >= 200 && xhr.status < 300) {
                    resolve({
                        ok: true,
                        status: xhr.status,
                        json: function () {
                            return Promise.resolve(JSON.parse(xhr.responseText));
                        },
                    });
                } else {
                    //cannot use reject, because we should handle then in same approach
                    resolve({
                        ok: false,
                        status: xhr.status,
                        statusText: xhr.statusText,
                    });
                }
            }
        };

        xhr.open(options.method, url, true);

        if (options.headers) {
            for (const [key, value] of Object.entries(options.headers)) {
                xhr.setRequestHeader(key, value);
            }
        }

        let requestData = null;
        if (options.method === 'POST' && options.body) {
            xhr.setRequestHeader('Content-Type', 'application/json');
            requestData = JSON.stringify(options.body);
        }

        xhr.send(requestData);
    });
}

function coverTimestamp(time) {
    // Convert JavaScript timestamp to Date object
    const date = new Date(time);

    // Get components of the date
    const year = date.getUTCFullYear();
    const month = (date.getUTCMonth() + 1).toString().padStart(2, '0'); // Months are 0-indexed
    const day = date.getUTCDate().toString().padStart(2, '0');
    const hours = date.getUTCHours().toString().padStart(2, '0');
    const minutes = date.getUTCMinutes().toString().padStart(2, '0');
    const seconds = date.getUTCSeconds().toString().padStart(2, '0');

    // Create the Qt datetime format string
    const qtDateTimeFormat = `${year}-${month}-${day} ${hours}:${minutes}:${seconds}`;

    return qtDateTimeFormat;
}


class APIManager {
    constructor(baseURL) {
        this.baseURL = baseURL;
        this.serverUrl = null
        this.webid = null
        this.cancel = false
    }

    constructURLWithParams(url, queryParams) {
        const defaultParams = {
            device_platform:'webapp',
            aid:6383,
            channel:'channel_pc_web',
            cookie_enabled:true,
            webid:this.getWebid(),
        }
        const params = new URLSearchParams({...defaultParams, ...queryParams});
        return params.toString() ? `${url}?${params.toString()}` : url;
    }
    
    async get(endpoint, queryParams = {}, headers = {}) {
        if(endpoint.indexOf('https://')!=-1){
            this.serverUrl = endpoint
        }else{
            this.serverUrl = this.baseURL + endpoint
        }
        const url = this.constructURLWithParams(this.serverUrl, queryParams);
        const response = await makeRequest(url, {
            method: 'GET',
            headers: headers,
        });

        return this.handleResponse(response);
    }

    async post(endpoint, queryParams = {}, data, headers = {}) {
        if(endpoint.indexOf('https://')!=-1){
            this.serverUrl = endpoint
        }else{
            this.serverUrl = this.baseURL + endpoint
        }
        const url = this.constructURLWithParams(this.serverUrl, queryParams);
        const response = await makeRequest(url, {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json',
                ...headers,
            },
            body: JSON.stringify(data),
        });

        return this.handleResponse(response);
    }

    exists(obj){
        if(typeof obj == "undefined" || obj == null){
            return false
        }
        return true
    }

    async getAll(requestFunctor, resultFunctor, total, delay=200){
        let results = {}
        let hasmore = true
        let cursor = 0
        async function request(){
            return new Promise((resolve, reject) => {
                requestFunctor(cursor).then((result)=>{
                    //delay 500ms
                    setTimeout(() => resolve(result), delay)
                })
            })
        }
        while(hasmore && !this.cancel){
            let result = await request()
            if(result.status_code!=0){
                break;
            }
            results = resultFunctor(results, result)
            cursor = result.cursor || result.max_cursor
            hasmore = result.has_more
            //stop if total is set and results length is enough
            if((total && results.total>=total) || result.error){
                break;
            }
        }
        return results
    }

    async handleResponse(response) {
        if (!response.ok) {
            // const errorMessage = await response.text();
            //throw new Error(`API request failed with status ${response.status}.`);
            return {status_code:response.status}
        }

        try {
            const jsonResponse = await response.json();
            jsonResponse.status_code = jsonResponse.status_code || 0
            return jsonResponse;
        } catch (error) {
            throw new Error('Failed to parse JSON response');
        }
    }

    cancelAll(){
        this.cancel = true
    }

    reset(){
        this.cancel = false
    }

    getWebid(){
        if(this.webid){
            return this.webid
        }
        for (let index = 0; index < self.__pace_f.length; index++) {
            if(self.__pace_f[index].length > 1 && self.__pace_f[index][1].indexOf('user_unique_id"') !=-1){
                this.webid = /"user_unique_id":"(\d+)"/g.exec(self.__pace_f[index][1])[1]
                return this.webid
            }
        }
    }

    detailParser(result){
        if(result.status_code!=0 || this.exists(result.aweme_detail)==false){
            return {}
        }
        const aweme_detail = result.aweme_detail

        return {
            site:'douyin',
            subject: aweme_detail.desc,
            description: aweme_detail.desc,
            url: aweme_detail.video.play_addr.url_list[0],
            cover: aweme_detail.video.cover.url_list[0],
            duration: aweme_detail.video.duration,
            resolution: aweme_detail.video.width+'x'+aweme_detail.video.height,
            ratio: aweme_detail.video.ratio,
            create_time: coverTimestamp(aweme_detail.create_time*1000),
            type: 'mp4',
            size: aweme_detail.video.play_addr.data_size,
            vid: aweme_detail.aweme_id,
            pageurl:'https://www.douyin.com/video/'+aweme_detail.aweme_id,
            //user
            nickname: aweme_detail.author.nickname,
            signature: aweme_detail.author.signature,
            avatar: aweme_detail.author.avatar_thumb.url_list[0],
            sec_uid: aweme_detail.author.sec_uid,
            short_id: aweme_detail.author.short_id,
            real_uid: aweme_detail.author.uid,
            
            //statistics
            collect_count: aweme_detail.statistics.collect_count,
            comment_count: aweme_detail.statistics.comment_count,
            like_count: aweme_detail.statistics.digg_count,
            share_count: aweme_detail.statistics.share_count,
        }
    }

    commentParser(result){
        if(result.status_code!=0 || this.exists(result.comments) == false){
            return {}
        }
        const comments = result.comments

        return {
            comments: comments.map((comment)=>{
                return {
                    parent_id: comment.aweme_id,
                    cid: comment.cid,
                    text: comment.text,
                    create_time: coverTimestamp(comment.create_time*1000),
                    like: comment.digg_count,
                    reply_total: comment.reply_comment_total,
                    ip_label: comment.ip_label,
                    pageurl:'https://www.douyin.com/video/'+comment.aweme_id,
                    //user
                    nickname: comment.user.nickname,
                    signature: comment.user.signature,
                    avatar: comment.user.avatar_thumb.url_list[0],
                    sec_uid: comment.user.sec_uid,
                    short_id: comment.user.short_id,
                    real_uid: comment.user.uid,
                    site:'douyin',
                    //
                }
            }),
            page:{
                hasmore: result.has_more||false,
                cursor: result.cursor,
                total: result.total,
            },
            site:'douyin',
        }
    }

    videosParser(result, info=''){
        if(result.status_code!=0 || this.exists(result.aweme_list) == false){
            return {}
        }
        const aweme_list = result.aweme_list

        return {
            videos: aweme_list.map((aweme)=>{
                try{
                    return {
                        site:'douyin',
                        subject: aweme.desc,
                        description: aweme.desc,
                        url: aweme.video.play_addr.url_list.find(url=>url.indexOf('www.')!==-1),
                        cover: aweme.video.cover.url_list[0],
                        duration: aweme.video.duration,
                        resolution: aweme.video.width+'x'+aweme.video.height,
                        ratio: aweme.video.ratio,
                        create_time: coverTimestamp(aweme.create_time*1000),
                        type: 'mp4',
                        size: aweme.video.play_addr.data_size,
                        vid: aweme.aweme_id,
                        pageurl:'https://www.douyin.com/video/'+aweme.aweme_id,
                        //user
                        nickname: aweme.author.nickname,
                        signature: aweme.author.signature,
                        avatar: aweme.author.avatar_thumb.url_list[0],
                        sec_uid: aweme.author.sec_uid,
                        short_id: aweme.author.short_id,
                        real_uid: aweme.author.uid,
                        //statistics
                        collect_count: aweme.statistics.collect_count,
                        comment_count: aweme.statistics.comment_count,
                        like_count: aweme.statistics.digg_count,
                        share_count: aweme.statistics.share_count,
                    }
                }catch(err){
                    //sometime live will have no video
                    return null
                }

            }).filter(item => item !== null),
            page:{
                hasmore: result.has_more||false,
                max_cursor: result.max_cursor||0,
                cursor: result.cursor||0,
            },
            site:'douyin',
            resourceView:true,
            info:info,
        }
    }

    searchParser(result, info=''){
        if(result.status_code!=0){
            return {}
        }
        const aweme_list = result.data

        return {
            videos: aweme_list.map((aweme)=>{
                if(!aweme.aweme_info){
                    return null
                }
                aweme = aweme.aweme_info
                try{
                    return {
                        site:'douyin',
                        subject: aweme.desc,
                        description: aweme.desc,
                        url: aweme.video.play_addr.url_list[0],
                        cover: aweme.video.cover.url_list[0],
                        duration: aweme.video.duration,
                        resolution: aweme.video.width+'x'+aweme.video.height,
                        ratio: aweme.video.ratio,
                        create_time: coverTimestamp(aweme.create_time*1000),
                        type: 'mp4',
                        size: aweme.video.play_addr.data_size,
                        vid: aweme.aweme_id,
                        pageurl:'https://www.douyin.com/video/'+aweme.aweme_id,
                        //user
                        nickname: aweme.author.nickname,
                        avatar: aweme.author.avatar_thumb.url_list[0],
                        sec_uid: aweme.author.sec_uid,
                        short_id: aweme.author.short_id,
                        real_uid: aweme.author.uid,
                        //statistics
                        collect_count: aweme.statistics.collect_count,
                        comment_count: aweme.statistics.comment_count,
                        like_count: aweme.statistics.digg_count,
                        share_count: aweme.statistics.share_count,
                    }
                }catch(err){
                    //sometime live will have no video
                    return null
                }

            }).filter(item => item !== null),
            page:{
                hasmore: result.has_more||false,
                cursor: result.cursor||0,
            },
            site:'douyin',
            resourceView:true,
            info:info,
        }
    }

    async video_detail(aweme_id){
        return this.get('/aweme/detail/', {aweme_id:''+aweme_id}, {Referer:'https://www.douyin.com/video/'+aweme_id})
    }

    async video_comments(aweme_id, cursor=0, count=50){
        return this.get('/comment/list/', {aweme_id:''+aweme_id, cursor:cursor, count:count, item_type:0}, {Referer:'https://www.douyin.com/video/'+aweme_id})
    }

    // get all comments
    // total: 0 for all
    async video_comments_all(aweme_id, total=100){
        return this.getAll((cursor)=>this.video_comments(aweme_id, cursor), 
        (obj, result)=>{
            if(result.comments){
                if(!obj.comments){
                    obj.comments = [...result.comments]
                }else{
                    obj.comments = obj.comments.concat(result.comments)
                }
                obj.has_more = result.has_more
                obj.cursor = result.cursor
                obj.status_code = result.status_code
                obj.total = obj.comments.length
            }else{
                obj.error = 1
            }
            return obj
            
        }, total)
    }

    // total: 0 for all
    async user_videos_all(sec_user_id, total=200){
        /** 
        let results = []
        let max_cursor = 0
        let hasmore = true
        while(hasmore){
            let result = await this.user_videos(sec_user_id, max_cursor, 38)
            if(result.status_code!=0){
                break;
            }
            results = results.concat(result.aweme_list)
            max_cursor = result.max_cursor
            hasmore = result.has_more
        }
        return {
            aweme_list:results,
            has_more:false,
            max_cursor:max_cursor,
            status_code:0,
        }
        */
        return this.getAll((cursor)=>this.user_videos(sec_user_id, cursor), 
        (obj, result)=>{
            if(result.aweme_list){
                if(!obj.aweme_list){
                    obj.aweme_list = [...result.aweme_list]
                }else{
                    obj.aweme_list = obj.aweme_list.concat(result.aweme_list)
                }
                obj.has_more = result.has_more
                obj.max_cursor = result.max_cursor
                obj.status_code = result.status_code
                obj.total = obj.aweme_list.length
    
            }else{
                obj.error = 1
            }
            return obj
        }, total)
    }

    async user_videos(sec_user_id, max_cursor=0, count=50){
        return this.get('/aweme/post/', {sec_user_id:sec_user_id, max_cursor:max_cursor, count:count, locate_query:false}, {Referer:'https://www.douyin.com/user/'+sec_user_id})
    }

    async related_videos(aweme_id, count=30){
        return this.get('/aweme/related/', {aweme_id:''+aweme_id, filterGids:''+aweme_id, count:count}, {Referer:'https://www.douyin.com/video/'+aweme_id})
    }

    async search_videos(keyword, count=30){
        return this.get('/general/search/single/', {keyword:keyword, count:count, search_source:'search_history', query_correct_type:1, is_filter_search:0, search_channel:'aweme_general', sort_type:0, publish_time:0}, {Referer:'https://www.douyin.com/search/'+encodeURIComponent(keyword)})
    }

    async channel(tag_id, count=50){
        return this.get('/channel/feed/', {tag_id:tag_id, count:count, cursor:0}, {Referer:'https://www.douyin.com/channel/'+tag_id})
    }

    async hotsport(count=50){
        //require login
        return this.get('/channel/hotsport/', {count:count, tag_id:'', channel_id:99, 'Seo-Flag':0, }, {Referer:'https://www.douyin.com/hot'})
    }

    async collection(mix_id, cursor=0, count=50){
        return this.get('/mix/aweme/', {mix_id:mix_id, cursor:cursor, count:count}, {Referer:'https://www.douyin.com/video/'+mix_id})
    }

    async user(sec_user_id){
        return this.get('/mix/list/', {sec_user_id:sec_user_id, count:6, cursor:0}, {Referer:'https://www.douyin.com/user/'+sec_user_id})
    }

    async signature(encodeString){
        return window.byted_acrawler.frontierSign({ "X-MS-STUB": encodeString})
    }

    //https://live.douyin.com/webcast/im/fetch/?
    async liveConnection(user_unique_id, room_id){
        return this.get('https://live.douyin.com/webcast/im/fetch/', 
            {user_unique_id:user_unique_id, 
                room_id:room_id,
                did_rule:3,
                app_name:'douyin_web',
                endpoint:'live_pc',
                support_wrds:1,
                identity:'audience',
                need_persist_msg_count:15,
                version_code:180800}, {Referer:'https://www.douyin.com/live/'+room_id})
    }

}

const apiManager = new APIManager('https://www.douyin.com/aweme/v1/web');

setTimeout(()=>{
    window.process.call.connect(function(api, params){
        ApiDispather(api, params)
    })
}, 1000)

const ApiDispather = (api, params, scriptid)=>{
    apiManager.reset()
    switch(api){
        case "videoDetail":
            apiManager.video_detail(params.aweme_id).then(results=>{
                //return js results
                window.process.jsResult(apiManager.detailParser(results), scriptid)
            })
            break;
        case "videoComment":
            /**
            apiManager.video_comments(params.aweme_id, params.cursor, params.count).then(results=>{
                //return js results
                window.process.jsResult(apiManager.commentParser(results), scriptid)
            })
             */
            apiManager.video_comments_all(params.aweme_id, params.total).then(results=>{
                //return js results
                window.process.jsResult(apiManager.commentParser(results), scriptid)
            })
            break;
        case "userVideos":
            /** 
            apiManager.user_videos(params.sec_uid, params.max_cursor, params.count).then(results=>{
                window.process.jsResult(apiManager.videosParser(results, 'user'), scriptid)
            })
            */
            apiManager.user_videos_all(params.sec_uid).then(results=>{
                //return js results
                window.process.jsResult(apiManager.videosParser(results, 'user'), scriptid)
            })
            break;
        case "relatedVideos":
            apiManager.related_videos(params.aweme_id, params.count).then(results=>{
                //return js results
                window.process.jsResult(apiManager.videosParser(results), scriptid)
            })
            break;
        case "searchVideos":
            apiManager.search_videos(params.keyword, params.count).then(results=>{
                //return js results
                window.process.jsResult(apiManager.searchParser(results, params.keyword), scriptid)
            })
            break;
        case "channel":
            apiManager.channel(params.channel_id, params.count).then(results=>{
                //return js results
                window.process.jsResult(apiManager.videosParser(results), scriptid)
            })
            break;
        case "hostspot":
            apiManager.hotsport(params.count).then(results=>{
                //return js results
                window.process.jsResult(apiManager.videosParser(results), scriptid)
            })
            break;
        case "collection":
            apiManager.collection(params.mix_id, params.cursor, params.count).then(results=>{
                //return js results
                window.process.jsResult(apiManager.videosParser(results), scriptid)
            })
            break;
        case "cancel":
            apiManager.cancelAll()
            break;
    }
}


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

function getOrignalImageUrl(url) {
    var parts = url.split('/');
    var extractedString = parts[parts.length - 1];
    if(url.indexOf('spectrum')!=-1){
        extractedString = 'spectrum/' + extractedString;
    }
    return `https://sns-img-hw.xhscdn.com/${extractedString.split('!')[0]}?imageView2/2/w/format/png`
}

async function overrideJs()
{   
    function getRunTimeJs(){
        let scripts = document.getElementsByTagName('script')
        for(let i=0; i<scripts.length; i++){
            if(scripts[i].src.indexOf('runtime-main')>-1){
                return scripts[i].src
            }
        }
    }
    function executeJs(url){
        return new Promise((resolve)=>{
            fetch(url).then((res)=>{
                return res.text()
            }).then((text)=>{
                resolve(eval(text))
            })
        })
    }

    let funcIndex = 0
    for (const key in self.webpackChunkxhs_pc_web[0][1]) {
        if (self.webpackChunkxhs_pc_web[0][1][key].toString().indexOf('/api/sns/web/v1/search/hotlist') > -1) {
            funcIndex = key
            break;
        }
    }
    const func = self.webpackChunkxhs_pc_web[0][1][funcIndex]
    self.webpackChunkxhs_pc_web[0][1][funcIndex] = new Function('return '+func.toString().replace(/([a-zA-Z0-9_]+)=([a-zA-Z0-9_]+)}/, "$1=$2,window.axios_=$1}"))();
    
    return new Promise((resolve)=>{
        const runtimeJs = getRunTimeJs()
        executeJs(runtimeJs).then(()=>{
            console.log('== Injected runtime js ==')
            resolve()
        })
    })

}

class APIManager {
    constructor() {
        overrideJs().then(()=>{
            this.axios = window.axios_
        })
        this.islogin = false
    }

    async get(endpoint, queryParams = {}, headers = {}) {
        return window.axios_.get(endpoint, queryParams);
    }

    async post(endpoint, queryParams = {}) {
        return window.axios_.get(endpoint,);
    }

    
    feedParser(result){
        if(!result || !result.items){
            return {}
        }
        const item = result.items[0]
        const card = item.noteCard
        const video = card.video;
        const author = card.user;
        const statistics = card.interactInfo;
        const imageList = card.imageList || [];
        let resolution = imageList[0].width + 'x' + imageList[0].height;
        if(video){
            resolution = video.media.stream.h264[0].width + 'x' + video.media.stream.h264[0].height
        }
        let url = getOrignalImageUrl(imageList[0].urlDefault)
        if(video){
            url = 'https://sns-video-bd.xhscdn.com/'+video.consumer.originVideoKey
        }
        return {
            site: 'xiaohongshu',
            subject: card.title,
            description: card.desc,
            url: url,
            cover: imageList[0].urlDefault,
            duration: video?video.media.stream.h264[0].duration:'',
            resolution: resolution,
            ratio: '',
            create_time: coverTimestamp(card.time),
            type: card.type == "normal"? 'png' : 'mp4',
            size: video?video.media.stream.h264[0].size:'',
            vid: item.id,
            pageurl: 'https://www.xiaohongshu.com/explore/' + item.id,
            nickname: author.nickname,
            signature: '',
            avatar: author.avatar,
            object: JSON.stringify(imageList),
            sec_uid: '',
            short_id: '',
            real_uid: author.userId,
            collect_count: parseInt(statistics.collectedCount),
            comment_count: parseInt(statistics.commentCount),
            like_count: parseInt(statistics.likedCount),
            share_count: parseInt(statistics.shareCount)
        }

    }

    commentParser(result){
        if(!result || !result.comments){
            return {}
        }
        const comments = result.comments.map(item=>{
            return {
                site:'xiaohongshu',
                parent_id: item.noteId,
                cid: item.id,
                text: item.content,
                create_time: coverTimestamp(item.createTime),
                like: parseInt(item.likeCount),
                reply_total: parseInt(item.subCommentCount),
                ip_label: item.ipLocation,
                pageurl:'https://www.xiaohongshu.com/explore/'+item.noteId,
                //user
                nickname: item.userInfo.nickname,
                signature: '',
                avatar: item.userInfo.image || '',
                sec_uid: '',
                short_id: '',
                real_uid: item.userInfo.userId,
            }
        })
        return {
            comments: comments,
            page: {
                hasmore: result.hasMore||false,
                cursor: result.cursor,
            }
        }
    }

    userParser(result, info='') {
        let items = result.notes;
        if (!result || !items || items.length === 0) {
            return {};
        }
    
        const videos = items.map(item => {

            const user = item.user;
            const cover = item.cover;
            const imageList = item.imageList || "";
            const interactInfo = item.interactInfo || {};
    
            return {
                site: 'xiaohongshu',
                subject: item.displayTitle || '',
                description: item.displayTitle || '',
                url: getOrignalImageUrl(cover.urlDefault),
                cover: cover.urlDefault || '',
                // Assuming first image as cover
                duration: '', // No duration info available
                resolution: cover.width + 'x' + cover.height || '',
                ratio: '', // No ratio info available
                create_time: '', // No create time info available
                type: item.type == "normal"? 'png' : 'mp4',
                size: '', // No size info available
                vid: item.noteId || '',
                pageurl: 'https://www.xiaohongshu.com/explore/'+item.noteId, // No page URL info available
                // User
                nickname: user.nickName || user.nickname || '',
                avatar: user.avatar || '',
                sec_uid: '', // No sec_uid info available
                short_id: '', // No short_id info available
                real_uid: user.userId || '',
                // Statistics
                //object: JSON.stringify(imageList),
                collect_count: 0,
                comment_count: 0, // No comment count info available
                like_count: interactInfo ? parseInt(interactInfo.likedCount) : 0, // No like count info available
                share_count: 0, // No share count info available
            };
        }).filter(item => item !== null);
    
        return {
            videos: videos,
            page: {
                hasmore: result.hasMore || false,
                cursor: result.cursor || 0,
            },
            site: 'xiaohongshu',
            resourceView: true,
            info: info,
        };
    }

    searchParser(result, info='') {
        let items = result.items
        if (!result || !items || items.length === 0) {
            return {};
        }
    
        const videos = items.map(item => {
            if (!item.noteCard) {
                return null;
            }
    
            const user = item.noteCard.user;
            const cover = item.noteCard.cover;
            const imageList = item.noteCard.imageList || "";
            const interactInfo = item.noteCard.interactInfo || {};

            return {
                site: 'xiaohongshu',
                subject: item.noteCard.displayTitle || '',
                description: item.noteCard.displayTitle || '',
                url: getOrignalImageUrl(cover.urlDefault),
                cover: cover.urlDefault || '',
                // Assuming first image as cover
                duration: '', // No duration info available
                resolution: cover.width + 'x' + cover.height || '',
                ratio: '', // No ratio info available
                create_time: '', // No create time info available
                type: item.noteCard.type == "normal"? 'png' : 'mp4',
                size: '', // No size info available
                vid: item.id || '',
                pageurl: 'https://www.xiaohongshu.com/explore/'+item.id, // No page URL info available
                // User
                nickname: user.nickName || user.nickname || '',
                avatar: user.avatar || '',
                sec_uid: '', // No sec_uid info available
                short_id: '', // No short_id info available
                real_uid: user.userId || '',
                // Statistics
                object: JSON.stringify(imageList),
                collect_count: 0,
                comment_count: 0, // No comment count info available
                like_count: interactInfo ? parseInt(interactInfo.likedCount) : 0, // No like count info available
                share_count: 0, // No share count info available
            };
        }).filter(item => item !== null);
    
        return {
            videos: videos,
            page: {
                hasmore: result.hasMore || false,
                cursor: result.cursor || 0,
            },
            site: 'xiaohongshu',
            resourceView: true,
            info: info,
        };
    }
    

    search_id(){
        //search id, get from main.647b549.js
        var t;
        if (null === (t = window) || void 0 === t || !t.BigInt)
            return "";
        var e = BigInt(Date.now())
            , r = BigInt(Math.ceil(2147483646 * Math.random()));
        return e <<= BigInt(64),
        (e += r).toString(36)
    }

    async loginStatus() {
        return this.get('/api/loginStatus');
    }

    async qrcode_create(t) {
        var e = {};
        return e.summary = "web登录-创建二维码",
        this.axios.post("/api/sns/web/v1/login/qrcode/create", t, e)
    }
    async u() {
        var t = {};
        return t.summary = "web登录-获取二维码状态",
        this.axios.get("/api/sns/web/v1/login/qrcode/status", t)
    }
    async s(t) {
        var e = {};
        return e.summary = "web登录-用户激活",
        this.axios.post("/api/sns/web/v1/login/activate", t, e)
    }
    async c() {
        var t = {};
        return t.summary = "web登录-用户退登",
        this.axios.get("/api/sns/web/v1/login/logout", t)
    }
    async l() {
        var t = {};
        return t.summary = "web登录-发送验证码",
        this.axios.get("/api/sns/web/v1/login/send_code", t)
    }
    async f() {
        var t = {};
        return t.summary = "web登录-验证验证码",
        this.axios.get("/api/sns/web/v1/login/check_code", t)
    }
    async p(t) {
        var e = {};
        return e.summary = "web登录-验证码登录",
        this.axios.post("/api/sns/web/v1/login/code", t, e)
    }
    async h() {
        var t = {};
        return t.summary = "【web】获取登录后的真实点赞数",
        this.axios.get("/api/sns/web/v1/get_liked_num", t)
    }
    async homefeed(channel_id, cursor_score="") {
        const t = {"cursor_score":cursor_score,"num":39,"refresh_type":1,"note_index":39,"unread_begin_note_id":"","unread_end_note_id":"","unread_note_count":0,"category":channel_id,"search_key":"","need_num":14,"image_formats":["jpg","webp","avif"],"need_filter_image":false}
        var e = {};
        return e.summary = "【web】- homefeed",
        this.axios.post("/api/sns/web/v1/homefeed", t, e)
    }
    async get_feed(source_note_id) {
        var t = {"source_note_id":source_note_id,"image_formats":["jpg","webp","avif"],"extra":{"need_body_topic":"1"}}
        var e = {};
        return e.summary = "【web】- feed",
        this.axios.post("/api/sns/web/v1/feed", t, e)
    }
    async note_uncollect(t) {
        var e = {};
        return e.summary = "web笔记取消收藏",
        this.axios.post("/api/sns/web/v1/note/uncollect", t, e)
    }
    async note_collect(t) {
        var e = {};
        return e.summary = "web笔记收藏",
        this.axios.post("/api/sns/web/v1/note/collect", t, e)
    }
    async note_dislike(t) {
        var e = {};
        return e.summary = "WEB-笔记取消点赞",
        this.axios.post("/api/sns/web/v1/note/dislike", t, e)
    }
    async note_like(t) {
        var e = {};
        return e.level = "S2",
        e.summary = "WEB-笔记点赞",
        this.axios.post("/api/sns/web/v1/note/like", t, e)
    }
    async b() {
        var t = {};
        return t.summary = "web  表情列表 版本",
        this.axios.get("/api/im/redmoji/version", t)
    }
    async _() {
        var t = {};
        return t.summary = "web  表情列表",
        this.axios.get("/api/im/redmoji/detail", t)
    }
    async comment_delete(t) {
        var e = {};
        return e.summary = "web-删除评论",
        this.axios.post("/api/sns/web/v1/comment/delete", t, e)
    }
    async comment_post(t) {
        var e = {};
        return e.summary = "web-创建评论",
        this.axios.post("/api/sns/web/v1/comment/post", t, e)
    }
    async comment_dislike(t) {
        var e = {};
        return e.summary = "web-评论取消点赞",
        this.axios.post("/api/sns/web/v1/comment/dislike", t, e)
    }
    async comment_like(t) {
        var e = {};
        return e.summary = "web-评论点赞",
        this.axios.post("/api/sns/web/v1/comment/like", t, e)
    }
    async S() {
        var t = {};
        return t.summary = "web At用户列表",
        this.axios.get("/api/sns/web/v1/intimacy/intimacy_list", t)
    }
    async A() {
        var t = {};
        return t.summary = "web查询At搜索用户",
        this.axios.get("/api/sns/web/v1/intimacy/intimacy_list/search", t)
    }
    async follow(t) {
        var e = {};
        return e.summary = "web 用户关注",
        this.axios.post("/api/sns/web/v1/user/follow", t, e)
    }
    async unfollow(t) {
        var e = {};
        return e.summary = "web 用户取消关注",
        this.axios.post("/api/sns/web/v1/user/unfollow", t, e)
    }
    async subcomment_page() {
        var t = {};
        return t.summary = "web-查询二级评论",
        this.axios.get("/api/sns/web/v2/comment/sub/page", t)
    }
    async comment_page(noteId, cursor = "") {
        var t = {};
        t.params = {
            "noteId": noteId,
            "cursor": cursor,
            "topCommentId": "",
            "imageFormats": "jpg,webp,avif"
        }
        return t.summary = "web-查询一级评论列表",
        this.axios.get("/api/sns/web/v2/comment/page", t)
    }
    async selfinfo() {
        var t = {};
        return t.summary = "web个人页",
        this.axios.get("/api/sns/web/v1/user/selfinfo", t)
    }
    async otherinfo() {
        var t = {};
        return t.summary = "web他人页",
        this.axios.get("/api/sns/web/v1/user/otherinfo", t)
    }
    async P(t) {
        var e = {};
        return e.level = "S1",
        e.summary = "笔记详情页进入和退出时调取的指标上报接口-web",
        this.axios.post("/api/sns/web/v1/note/metrics_report", t, e)
    }
    async collect_page() {
        var t = {};
        return t.summary = "web-个人页收藏列表",
        this.axios.get("/api/sns/web/v2/note/collect/page", t)
    }
    async like_page() {
        var t = {};
        return t.summary = "web-个人页点赞列表",
        this.axios.get("/api/sns/web/v1/note/like/page", t)
    }
    async j() {
        var t = {};
        return t.summary = "【web】- homefeed_category",
        this.axios.get("/api/sns/web/v1/homefeed/category", t)
    }
    async F(t) {
        var e = {};
        return e.summary = "[web]-homefeed预热数据检查",
        this.axios.post("/api/sns/web/v1/check/noteids", t, e)
    }
    async D(t) {
        var e = {};
        return e.summary = "【web】-NPS",
        this.axios.post("/api/sns/web/v1/nps", t, e)
    }
    async U(t) {
        var e = {};
        return e.summary = "web端三方登录",
        this.axios.post("/api/sns/web/v1/login/social", t, e)
    }
    async search_notes(keyword, page = 1, note_type = 2) {
        // note_type 1 video 2 note 0 normal
        const t = {"keyword":decodeURI(keyword),"page":page,"page_size":20,"search_id":this.search_id(),"sort":"general","note_type":note_type,"ext_flags":[],"image_formats":["jpg","webp","avif"]}
        var e = {};
        return e.level = "S1",
        e.summary = "web笔记搜索",
        this.axios.post("/api/sns/web/v1/search/notes", t, e)
    }
    async search_filters() {
        var t = {};
        return t.level = "S1",
        t.summary = "筛选项接口",
        this.axios.get("/api/sns/web/v1/search/filters", t)
    }
    async user_me() {
        var t = {};
        return t.summary = "web用户-个人信息V2",
        this.axios.get("/api/sns/web/v2/user/me", t)
    }
    async querytrending() {
        var t = {};
        return t.summary = "web猜你想搜trending",
        this.axios.get("/api/sns/web/v1/search/querytrending", t)
    }
    async onebox(t) {
        var e = {};
        return e.summary = "onebox",
        this.axios.post("/api/sns/web/v1/search/onebox", t, e)
    }
    async user_postedV2() {
        var t = {};
        return t.summary = "【web】- user_posted v2",
        this.axios.get("/api/sns/web/v2/user_posted", t)
    }
    async user_posted(user_id, cursor = '', page_size = 30) {
        var t = {};
        t.params = {user_id:user_id, cursor:cursor, num:page_size, image_formats:"jpg,webp,avif"}
        return t.summary = "【web】- user_posted",
        this.axios.get("/api/sns/web/v1/user_posted", t)
    }
    async search_user(t) {
        var e = {};
        return e.level = "S1",
        e.summary = "用户搜索",
        this.axios.post("/api/sns/web/v1/search/usersearch", t, e)
    }
    async search_recommend() {
        var t = {};
        return t.level = "S2",
        t.summary = "search-recommend",
        this.axios.get("/api/sns/web/v1/search/recommend", t)
    }
    async Z(t) {
        var e = {};
        return e.summary = "web短信验证码登录注册",
        this.axios.post("/api/sns/web/v2/login/code", t, e)
    }
    async edit_user_info(t) {
        var e = {};
        return e.summary = "web端编辑资料",
        this.axios.post("/api/sns/web/v1/user/info", t, e)
    }
    async rt(t) {
        var e = {};
        return e.summary = "web上报注册兴趣",
        this.axios.post("/api/sns/web/v1/tag/follow", t, e)
    }
    async nio_feed(t) {
        var e = {};
        return e.summary = "nio笔详feed",
        this.axios.post("/api/sns/web/nio/feed", t, e)
    }
    async nio(t) {
        var e = {};
        return e.summary = "nio视频精选首页",
        this.axios.post("/api/sns/web/nio/init", t, e)
    }
    async sys_config() {
        var t = {};
        return t.summary = "web系统配置",
        this.axios.get("/api/sns/web/v1/system/config", t)
    }
    async send_code() {
        var t = {};
        return t.summary = "web登录-发送验证码v2",
        this.axios.get("/api/sns/web/v2/login/send_code", t)
    }
    async hotlist() {
        var t = {};
        return t.level = "S2",
        t.summary = "web_hot_list",
        this.axios.get("/api/sns/web/v1/search/hotlist", t)
    }
}

const apiManager = new APIManager();

const ApiDispather = async (api, params, scriptid)=>{
    if(apiManager.islogin==false){
        await apiManager.user_me().then((res)=>{
            if(!res.guest){
                apiManager.islogin = true
            }
        })
        if(apiManager.islogin==false){
            window.process.jsResult({loginView:true, site:'xiaohongshu'}, scriptid)
            return
        }
    }


    switch(api){
        case 'videoDetail':
            apiManager.get_feed(params.note_id).then((res)=>{
                window.process.jsResult(apiManager.feedParser(res), scriptid)
            });
            break
        case 'searchVideos':
            apiManager.search_notes(params.keyword).then((res)=>{
                window.process.jsResult(apiManager.searchParser(res), scriptid)
            })
            break
        case 'userInfo':
            apiManager.user_me().then((res)=>{
                window.process.jsResult(res, scriptid)
            })
            break;
        case 'videoComment':
            apiManager.comment_page(params.note_id).then((res)=>{
                window.process.jsResult(apiManager.commentParser(res), scriptid)
            })
            break;
        case 'userVideos':
            apiManager.user_posted(params.user_id, params.cursor, params.page_size).then((res)=>{
                window.process.jsResult(apiManager.userParser(res), scriptid)
            })
            break;
    }
}


setTimeout(()=>{
    window.process.call.connect(function(api, params){
        ApiDispather(api, params)
    })
}, 1000)
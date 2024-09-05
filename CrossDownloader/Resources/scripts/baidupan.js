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

class APIManager {
    constructor(baseURL) {
        this.baseURL = baseURL;
        this.webid = null
    }

    constructURLWithParams(url, queryParams) {
        const defaultParams = {
            clienttype:0,
            app_id:250528,
            web:1,
            channel:'chunlei',
        }
        const params = new URLSearchParams({...defaultParams, ...queryParams});
        return params.toString() ? `${url}?${params.toString()}` : url;
    }

    async get(endpoint, queryParams = {}, headers = {}) {
        const url = this.constructURLWithParams(this.baseURL + endpoint, queryParams);
        const response = await makeRequest(url, {
            method: 'GET',
            headers: headers,
        });

        return this.handleResponse(response);
    }

    async post(endpoint, queryParams = {}, data, headers = {}) {
        const url = this.constructURLWithParams(this.baseURL + endpoint, queryParams);
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
 
    async loginStatus() {
        return this.get('/api/loginStatus');
    }
}

const apiManager = new APIManager('https://pan.baidu.com');

const ApiDispather = (api, params, scriptid)=>{
    switch(api){
        case 'loginStatus':
            apiManager.loginStatus().then((result)=>{
                window.process.jsResult(result, scriptid)
            })
            break
        case 'getSign':
            apiManager.get('/api/getSign', params).then((res)=>{
                window.process.call.callback(scriptid, res)
            })
            break
        case 'getShareInfo':
            apiManager.get('/share/wxlist', params).then((res)=>{
                window.process.call.callback(scriptid, res)
            })
            break
        
    }
}


setTimeout(()=>{
    window.process.call.connect(function(api, params){
        ApiDispather(api, params)
    })
}, 1000)
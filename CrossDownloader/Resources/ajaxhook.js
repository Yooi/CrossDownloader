!function(e,t){for(var n in t)e[n]=t[n]}(window,function(n){var r={};function o(e){if(r[e])return r[e].exports;var t=r[e]={i:e,l:!1,exports:{}};return n[e].call(t.exports,t,t.exports,o),t.l=!0,t.exports}return o.m=n,o.c=r,o.i=function(e){return e},o.d=function(e,t,n){o.o(e,t)||Object.defineProperty(e,t,{configurable:!1,enumerable:!0,get:n})},o.n=function(e){var t=e&&e.__esModule?function(){return e.default}:function(){return e};return o.d(t,"a",t),t},o.o=function(e,t){return Object.prototype.hasOwnProperty.call(e,t)},o.p="",o(o.s=3)}([function(e,t,n){"use strict";Object.defineProperty(t,"__esModule",{value:!0});var a="function"==typeof Symbol&&"symbol"==typeof Symbol.iterator?function(e){return typeof e}:function(e){return e&&"function"==typeof Symbol&&e.constructor===Symbol&&e!==Symbol.prototype?"symbol":typeof e};t.configEvent=l,t.hook=function(s,e){var u=(e=e||window).XMLHttpRequest;return e.XMLHttpRequest=function(){for(var e,t=new u,n=0;n<c.length;++n){var r="on"+c[n];void 0===t[r]&&(t[r]=null)}for(e in t){var o="";try{o=a(t[e])}catch(e){}"function"===o?this[e]=function(n){return function(){var e=[].slice.call(arguments);if(s[n]){var t=s[n].call(this,e,this[f]);if(t)return t}return this[f][n].apply(this[f],e)}}(e):e!==f&&Object.defineProperty(this,e,{get:function(n){return function(){var e=this.hasOwnProperty(n+"_")?this[n+"_"]:this[f][n],t=(s[n]||{}).getter;return t&&t(e,this)||e}}(e),set:function(o){return function(t){var n=this[f],r=this,e=s[o];if("on"===o.substring(0,2))r[o+"_"]=t,n[o]=function(e){e=l(e,r),s[o]&&s[o].call(r,n,e)||t.call(r,e)};else{e=(e||{}).setter;t=e&&e(t,r)||t,this[o+"_"]=t;try{n[o]=t}catch(e){}}}}(e),enumerable:!0})}var i=this;t.getProxy=function(){return i},this[f]=t},Object.assign(e.XMLHttpRequest,{UNSENT:0,OPENED:1,HEADERS_RECEIVED:2,LOADING:3,DONE:4}),{originXhr:u,unHook:function(){e.XMLHttpRequest=u,u=void 0}}};var c=t.events=["load","loadend","timeout","error","readystatechange","abort"],f="__origin_xhr";function l(e,t){var n,r={};for(n in e)r[n]=e[n];return r.target=r.currentTarget=t,r}},function(e,t,n){"use strict";Object.defineProperty(t,"__esModule",{value:!0}),t.proxy=function(e,t){return t=t||window,function(e,t){var o=e.onConfig,i=null,s=e.onRequest,u=e.onResponse,a=e.onError;function n(){return!0}function r(o){return function(e,t){var n,r;return n=t,r=o,e=new m(t=e),n={config:t.config,error:n,type:r},a?a(n,e):e.next(n),!0}}function c(e,t){var n=e?e.config:null;return n&&e&&n.xhr===e&&(4===e.readyState&&0!==e.status?function(e,t){var n,r=new b(e),o=function(){var e=t.responseType;return e&&"text"!==e?"json"===e&&"object"==typeof JSON&&-1!==((navigator||0).userAgent||"").indexOf("Trident")?JSON.parse(t.responseText):t.response:t.responseText},e={get response(){return o&&(n=o(),o=null),n},set response(e){return n=e,!(o=null)},status:t.status,statusText:t.statusText,config:e.config,headers:e.resHeader||e.getAllResponseHeaders().split("\r\n").reduce(function(e,t){if(""===t)return e;t=t.split(":");return e[t.shift()]=t.join(":").replace(/^\s+|\s+$/g,""),e},{})};if(!u)return r.resolve(e);u(e,r)}(e,t):4!==e.readyState&&g(e,v)),!0}var f="function"==typeof WeakMap?function(e){return e.eventListenerFnMap||(e.eventListenerFnMap=new WeakMap)}:null,e=(0,l.hook)({onload:n,onloadend:n,onerror:r(d),ontimeout:r(h),onabort:r(p),onreadystatechange:function(e){return c(e,this)},open:function(e,t){var n=this,r=t.config={headers:{}};r.method=e[0],r.url=e[1],r.async=e[2],r.user=e[3],r.password=e[4],Object.defineProperty(r,"xhr",{get:function(){return t},set:function(e){return null===e&&(t=null),!0},enumerable:!1,configurable:!0});e="on"+v;if(t[e]||(t[e]=function(){return c(this,n)}),i=s,o&&!1===o(r,this)&&(i=null),i)return!0},send:function(e,t){var n=t.config;if(n.withCredentials=t.withCredentials,n.body=e[0],i){e=function(){i(n,new x(t))};return!1===n.async?e():setTimeout(e),!0}},setRequestHeader:function(e,t){if(t.config.headers[e[0].toLowerCase()]=e[1],i)return!0},addEventListener:function(e,t){var n=this;if(-1!==l.events.indexOf(e[0])){var r=e[1],o=function(e){e=l.configEvent(e,n);e.isTrusted=!0,r.call(n,e)};return f&&f(n).set(r,o),y(t).addEventListener(e[0],o,!1),!0}},removeEventListener:function(e,t){if(-1!==l.events.indexOf(e[0])){var n=e[1];if(f){n=f(this).get(n);if(n)return y(t).removeEventListener(e[0],n,!1),!0}}},getAllResponseHeaders:function(e,t){var n=t.resHeader;if(n){var r,o="";for(r in n)o+=r+": "+n[r]+"\r\n";return o}},getResponseHeader:function(e,t){t=t.resHeader;if(t)return t[(e[0]||"").toLowerCase()]}},t),t=e.originXhr,e=e.unHook;return{originXhr:t,unProxy:e}}(e,t)};var l=n(0),r=l.events[0],o=l.events[1],h=l.events[2],d=l.events[3],v=l.events[4],p=l.events[5],i="prototype";function y(e){return e.watcher||(e.watcher="function"==typeof document.createDocumentFragment?document.createDocumentFragment():document.createElement("a"))}function g(e,t){var n,r=e.getProxy(),o="on"+t+"_",i=(0,l.configEvent)({type:t},r);r[o]&&r[o](i),"function"==typeof Event?n=new Event(t,{bubbles:!1}):(n=document.createEvent("Event")).initEvent(t,!1,!0),y(e).dispatchEvent(n)}function s(e){this.xhr=e,this.xhrProxy=e.getProxy()}function u(e){function t(e){s.call(this,e)}return(t[i]=Object.create(s[i])).next=e,t}s[i]=Object.create({resolve:function(e){var t=this.xhrProxy,n=this.xhr;t.readyState=4,n.resHeader=e.headers,t.response=t.responseText=e.response,t.statusText=e.statusText,t.status=e.status,g(n,v),g(n,r),g(n,o),4===n.readyState&&(n.config&&(n.config.xhr=null),n["on"+v]=null,n.config=null)},reject:function(e){this.xhrProxy.status=0,g(this.xhr,e.type),g(this.xhr,o),4===xhr.readyState&&(xhr.config&&(xhr.config.xhr=null),xhr["on"+v]=null,xhr.config=null)}});var x=u(function(e){var t,n=this.xhr;for(t in e=e||n.config,n.withCredentials=e.withCredentials,n.open(e.method,e.url,!1!==e.async,e.user,e.password),e.headers)n.setRequestHeader(t,e.headers[t]);n.send(e.body)}),b=u(function(e){this.resolve(e)}),m=u(function(e){this.reject(e)})},,function(e,t,n){"use strict";Object.defineProperty(t,"__esModule",{value:!0}),t.ah=void 0;var r=n(0),n=n(1);t.ah={proxy:n.proxy,hook:r.hook}}]));
window.WebHook = ah
//intercept url and return result with callback
WebHook.Interceptor = {
    requests:{},  //key: url, val:callback
    responses:{}
}

WebHook.addHook = function(url, callback, resp=true){
    if(resp){
        WebHook.Interceptor.responses[url] = callback
    }else{
        WebHook.Interceptor.requests[url] = callback
    }
}

WebHook.removeHook = function(url, resp=true){
    if(resp){
        delete WebHook.Interceptor.responses[url]
    }else{
        delete WebHook.Interceptor.requests[url]
    }
}

function SVM_XHRHook(...args){
	if(args.length%2 != 0){
		//error
		return
	}
    //init Interceptor
    for (let i = 0; i < args.length; i += 2) {
        WebHook.addHook(args[i], args[i+1])
    }

	WebHook.proxy({
	  //请求成功后进入
	  onResponse: (response, handler) => {
	  	let url = response.config.url;
	  	console.log('---Handle URL '+url)
	  	for (const checkurl in WebHook.Interceptor.responses) {
	  		const callback = WebHook.Interceptor.responses[checkurl]
	  		if(url.indexOf(checkurl) != -1){
	  			let result = response.response
	  			if(typeof result == "string"){
	  				try{
						result = JSON.parse(result)
	  				}catch(err){

	  				}
	  			}
	  			callback(result, response.config)
	  			break
	  		}
	  	}

	    handler.next(response)
	  }
	})
}
function SVM_XHRUnHook(){
    WebHook.unProxy()
}


console.log('>>>create WebHook successful');
var SVM_interaction = {}

/*
    simulate user interaction in webpage, complitable with vue and react
*/
SVM_interaction.method = (function(){
    const inputElementWithString = function(selector, string){
        const element = document.querySelector(selector)
        // Iterate through the paragraph elements
        if (element) {
            // Check if the paragraph contains the specific string
            element.innerText = string
        }
    }
    const clickElementWithString = async function(selector, string){
        await waitForElement(selector)
        const elements = document.querySelectorAll(selector)
        // Iterate through the paragraph elements
        for (const element of elements) {
            // Check if the paragraph contains the specific string
            if (element.textContent.includes(string)) {
                // Trigger a click event on the paragraph
                element.click();
                break; // Remove this line if you want to click all matching elements
            }
        }
    }
    const clickPopoverElementWithString = async function(selector, container, string){
        await waitForElement(selector)
        document.querySelector(selector).click();
        // Iterate through the paragraph elements
        clickElementWithString(container, string);
    }

    const delayClick = async function(selector, string, delay){
        await waitForTime(delay)
        clickElementWithString(selector, string)
    }

    const waitForElement = function(selector) {
        return new Promise(resolve => {
            const element = document.querySelector(selector);
            if (element) {
                return resolve(element);
            }
    
            const observer = new MutationObserver(mutations => {
                // Query for elements matching the specified selector
                const el = document.querySelector(selector);
    
                if (el) {
                    resolve(el);
                    observer.disconnect();
                }
            });
    
            observer.observe(document.documentElement, {
                childList: true,
                subtree: true
            });
        });
    }
    
    const waitForElementToNotExist = function(selector) {
        return new Promise(resolve => {
            const element = document.querySelector(selector);
            if (!element) {
                return resolve();
            }
    
            const observer = new MutationObserver(mutations => {
                // Query for elements matching the specified selector
                const el = document.querySelector(selector);
    
                if (!el) {
                    resolve();
                    observer.disconnect();
                }
            });
    
            observer.observe(document.documentElement, {
                childList: true,
                subtree: true
            });
        });
    }
    
    const waitForTime = function(time){
        return new Promise(resolve => {
            setTimeout(()=>{
                resolve()
            }, time)
        })
    }

    const inputAfterEnter = async function(selector, string){
        await waitForElement(selector)
        const element = document.querySelector(selector)
        element.focus()
        // Iterate through the paragraph elements
        if (element) {
            // Check if the paragraph contains the specific string
            simulateInput(element, string)
            element.dispatchEvent(new KeyboardEvent('keydown',{'key':'Enter', keyCode: 13, bubbles: true }))
        }
    }
    
    
    //because of the react, we need to simulate the input event
    //https://stackoverflow.com/questions/23892547/what-is-the-best-way-to-trigger-onchange-event-in-react-js
    //because after react >= 16, the event is not triggered by the native event
    const simulateInput = function(element, value) {
        const nativeInputValueSetter = Object.getOwnPropertyDescriptor(window.HTMLInputElement.prototype, 'value').set;
        nativeInputValueSetter.call(element, value);
        
        // hack React15
        // fire the event input and change
        element.dispatchEvent(new Event('input', { bubbles: true, cancelable: true }));
        element.dispatchEvent(new Event('change', { bubbles: true, cancelable: true }));
    }

    return {
        click:clickElementWithString,
        clickPopover:clickPopoverElementWithString,
        wait:waitForElement,
        waitNotExist:waitForElementToNotExist,
        sleep:waitForTime,
        delayClick:delayClick,
        input:inputElementWithString,
        inputAfterEnter:inputAfterEnter
    }

})()


var SVM_clickElementWithStr = SVM_interaction.method.click;
var SVM_waitForElement = SVM_interaction.method.wait;
var SVM_waitForNoElement = SVM_interaction.method.waitNotExist;
var SVM_sleep = SVM_interaction.method.sleep;
var SVM_delayClick = SVM_interaction.method.delayClick;
var SVM_addHook = WebHook.addHook;
var SVM_removeHook = WebHook.removeHook;

var h = !1,
    q = RegExp("^bytes [0-9]+-[0-9]+/([0-9]+)$"),
    v = "object xmlhttprequest media other main_frame sub_frame image".split(" "),
    y = ["object", "xmlhttprequest", "media", "other"],
    z = RegExp("://.+/([^/]+?(?:.([^./]+?))?)(?=[?#]|$)"),
    aa = [301, 302, 303, 307, 308],
    ba = RegExp("^(?:application/x-apple-diskimage|application/download|application/force-download|application/x-msdownload|binary/octet-stream)$", "i"),
    A = RegExp("^(?:FLV|SWF|MP3|MP4|M4V|F4F|F4V|M4A|M4S|MPG|MPEG|MPEG4|MPE|AVI|WMV|WMA|WAV|WAVE|ASF|RM|RAM|OGG|OGV|OGM|OGA|MOV|MID|MIDI|3GP|3GPP|QT|WEBM|TS|MKV|AAC|MP2T|MPEGTS|RMVB|VTT|SRT)$",
        "i"),
    ca = RegExp("^(?:HTM|HTML|MHT|MHTML|SHTML|SHTM|XHT|XHTM|XHTML|XML|TXT|CSS|JS|JSON|GIF|ICO|JPEG|JPG|PNG|WEBP|BMP|SVG|TIF|TIFF|PDF|PHP|ASP|ASPX|EOT|TTF|WOF|WOFF|WOFF2|MSG|PEM|BR|OTF|ACZ|AZC|CGI|TPL|OSD|M3U8|DO)$", "i"),
    da = RegExp("^(?:FLV|AVI|MPG|MPE|WMV|QT|MOV|RM|RAM|WMA|MID|MIDI|AAC|MKV|RMVB)$", "i"),
    C = RegExp("^(?:F4F|MPEGTS|TS|MP2T)$", "i"),
    MimeTypesMapping = {
        "application/x-apple-diskimage": "DMG",
        "application/cert-chain+cbor": "MSG",
        "application/epub+zip": "EPUB",
        "application/java-archive": "JAR",
        "video/x-matroska": "MKV",
        "text/html": "HTML|HTM",
        "text/css": "CSS",
        "text/javascript": "JS|JSON",
        "text/mspg-legacyinfo": "MSI|MSP",
        "text/plain": "TXT|SRT",
        "text/srt": "SRT",
        "text/vtt": "VTT|SRT",
        "text/xml": "XML|F4M|TTML",
        "text/x-javascript": "JS|JSON",
        "text/x-json": "JSON",
        "application/f4m+xml": "F4M",
        "application/gzip": "GZ",
        "application/javascript": "JS",
        "application/json": "JSON",
        "application/msword": "DOC|DOCX|DOT|DOTX",
        "application/pdf": "PDF",
        "application/ttaf+xml": "DFXP",
        "application/vnd.apple.mpegurl": "M3U8",
        "application/zip": "ZIP",
        "application/x-7z-compressed": "7Z",
        "application/x-aim": "PLJ",
        "application/x-compress": "Z",
        "application/x-compress-7z": "7Z",
        "application/x-compressed": "ARJ",
        "application/x-gtar": "TAR",
        "application/x-msi": "MSI",
        "application/x-msp": "MSP",
        "application/x-gzip": "GZ",
        "application/x-gzip-compressed": "GZ",
        "application/x-javascript": "JS",
        "application/x-mpegurl": "M3U8",
        "application/x-msdos-program": "EXE|DLL",
        "application/vnd.apple.installer+xml": "MPKG",
        "application/x-ole-storage": "MSI|MSP",
        "application/x-rar": "RAR",
        "application/x-rar-compressed": "RAR",
        "application/x-sdlc": "EXE|SDLC",
        "application/x-shockwave-flash": "SWF",
        "application/x-silverlight-app": "XAP",
        "application/x-subrip": "SRT",
        "application/x-tar": "TAR",
        "application/x-zip": "ZIP",
        "application/x-zip-compressed": "ZIP",
        "video/3gpp": "3GP|3GPP",
        "video/3gpp2": "3GP|3GPP",
        "video/avi": "AVI",
        "video/f4f": "F4F",
        "video/f4m": "F4M",
        "video/flv": "FLV",
        "video/mp2t": "TS|M3U8",
        "video/mp4": "MP4",
        "video/mpeg": "MPG|MPEG|MPE",
        "video/mpegurl": "M3U8",
        "video/mpg4": "MP4|M4V",
        "video/msvideo": "AVI",
        "video/quicktime": "MOV|QT",
        "video/webm": "WEBM",
        "video/x-flash-video": "FLV",
        "video/x-flv": "FLV",
        "video/x-mp4": "MP4|M4V",
        "video/x-mpegurl": "M3U8|M3U",
        "video/x-mpg4": "MP4|M4V",
        "video/x-ms-asf": "ASF",
        "video/x-ms-wmv": "WMV",
        "video/x-msvideo": "AVI",
        "audio/3gpp": "3GP|3GPP",
        "audio/3gpp2": "3GP|3GPP",
        "audio/mp3": "MP3",
        "audio/mp4": "M4A|MP4",
        "audio/mp4a-latm": "M4A|MP4",
        "audio/mpeg": "MP3",
        "audio/mpeg4-generic": "M4A|MP4",
        "audio/mpegurl": "M3U8|M3U",
        "image/svg+xml": "SVG|SVGZ",
        "audio/webm": "WEBM",
        "audio/wav": "WAV",
        "audio/x-mpeg": "MP3",
        "audio/x-mpegurl": "M3U8|M3U",
        "audio/x-ms-wma": "WMA",
        "audio/x-wav": "WAV",
        "ilm/tm": "MP3",
        "image/gif": "GIF|GFA",
        "image/icon": "ICO|CUR",
        "image/jpg": "JPG|JPEG",
        "image/jpeg": "JPG|JPEG",
        "image/png": "PNG|APNG",
        "image/tiff": "TIF|TIFF",
        "image/vnd.microsoft.icon": "ICO|CUR",
        "image/webp": "WEBP",
        "image/x-icon": "ICO|CUR",
        "flv-application/octet-stream": "FLV",
        "image/x-xbitmap": "XBM",
        "audio/x-mp3": "MP3",
        "audio/x-hx-aac-adts": "AAC",
        "audio/aac": "AAC",
        "audio/x-aac": "AAC",
        "application/vnd.rn-realmedia-vbr": "RMVB"
    };

function getContentType(a) {
    return a && unescape(a.split(";", 1).shift().trim()) || ""
}

function getFilename(a) {
    return (a = z.exec(a)) ? a[1] || "" : ""
}

function getFileType(a) {
    return -1 < a.indexOf(".") ? a.split(".").pop() : ""
}

function getMimeType(a) {
    var b;
    a = a.toUpperCase();
    for (b in MimeTypesMapping)
        if (-1 < MimeTypesMapping[b].split("|").indexOf(a)) return b;
    return ""
}

function L(a, b) {
    if (!a) return null;
    for (var c = 0; c < a.length; c++)
        if (a[c].name.toLowerCase() == b.toLowerCase()) return a[c].value || a[c].binaryValue || null;
    return null
}

function M() {
    for (var a = {}, b = 0; b < arguments.length; b++)
        for (var c in arguments[b]) arguments[b].hasOwnProperty(c) && (a[c] = arguments[b][c]);
    return a
}

function N(a, b) {
    return a && b && 0 == a.indexOf(b)
}

function O(a, b) {
    if (!a || !b) return !1;
    var c = a.length - b.length;
    return 0 <= c && a.indexOf(b, c) == c
}

function exists(a, b) {
    return a && b && 0 <= a.indexOf(b)
}

function getSchema(url) {
  try {
    const parsedUrl = new URL(url);
    return parsedUrl.protocol.replace(/:$/, '');
  } catch (error) {
    // If the URL parsing fails, handle the exception here
    console.error('Invalid URL:', error);
    return null;
  }
}

async function R(a, b) {
    var c = null,
        d = {},
        e, f = b && b["1"] || "GET";
    if (b && (e = b.m))
        for (var g = 0; g < e.length; g++) N(e[g].name.toLowerCase(), "x-") && (d[e[g].name] = e[g].value);
    if ("POST" == f && b) {
        try {
            S(b, b), b["10"] && (d["Content-Type"] = b["10"])
        } catch (m) {}
        b && b.postData && (c = b.postData)
    }
    try {
        const m = await fetch(a["2"], {
            method: f,
            credentials: "include",
            headers: new Headers(d),
            body: c
        });
        if (m.ok) {
            let t = await m.text();
            (a.S || function() {})(t)
        }
    } catch (m) {}
}

function DataConstuctor() {
    this["1"] = "GET";
    this["2"] = "";
    this["3"] = "";
    this["4"] = "";
    this["5"] = "";
    this["6"] = "normal";
    this["7"] = 0;
    this["8"] = "";
    this["9"] = "";
    this["10"] = "";
    this["12"] = "";
    this.cookies = this["11"] = "";
    this.postData = null
}

function SVMConstructor() {
    var a = this.constructor.prototype,
        b;
    for (b in a) this[b] = a[b].bind(this);
    this.H = {};
    this.h = {};
    this.l = {};
    this.fa = 1;
    this.s = "";
    this.C = !1;
    chrome.contextMenus.removeAll();
    chrome.contextMenus.create({
        title: "=CrossDownloader=",
        id: "SVMMenuItem",
        contexts: ["link", "image"]
    });
    this.j(chrome.contextMenus.onClicked, this.menuClicked);
    //this.j(chrome.downloads.onCreated, this.downloadsCreated);
    this.j(chrome.runtime.onConnect, this.runtimeConnect);
    this.j(chrome.webRequest.onBeforeRequest, this.onBeforeRequest, {
        urls: ["http://*/*", "https://*/*"],
        types: v
    }, ["requestBody"]);
    this.j(chrome.webRequest.onBeforeSendHeaders, this.onBeforeSendHeaders, {
        urls: ["https://*/*", "http://*/*"],
        types: v
    }, ["requestHeaders"]);
    this.j(chrome.webRequest.onHeadersReceived, this.onHeadersReceived, {
        urls: ["<all_urls>"],
        types: v
    }, ["responseHeaders"]);
    this.j(chrome.webRequest.onCompleted, this.onCompleted, {
        urls: ["<all_urls>"]
    });
    this.j(chrome.webRequest.onErrorOccurred, this.onCompleted, {
        urls: ["<all_urls>"]
    });
    this.j(chrome.webNavigation.onHistoryStateUpdated, this.onHistoryStateUpdated);
    chrome.action.onClicked.addListener(this.actionClicked);
    this.v = !1;
    chrome.action.setBadgeBackgroundColor({
        color: "#FF3333"
    });
    this.actionClicked();
    var c = this;
    this.F = !0;
    chrome.storage.local.get(["ShowMediaPanel"], function(d) {
        -1 == d.ShowMediaPanel && (c.F = !1)
    });
    this._data = this.G = null;
    this.D = !1;
    this.startUpSocket()
}
var V = SVMConstructor.prototype;
V.actionClicked = function() {
    var a = (this.v = !this.v) ? "" : "Off";
    chrome.action.setTitle({
        title: this.v ? "" : "Download catcher is Off\r\nClick to toggle catching"
    });
    chrome.action.setBadgeText({
        text: a
    })
};
V.onHistoryStateUpdated = function(a) {
    var b = this.h[[a.tabId, a.frameId]];
    b && b["2"] != a.url && (b.postMessage([11, a.url]), b["2"] = a.url)
};
V.downloadsCreated = function(a) {
    h || !this.v ? this.s = "" : this.s != a.finalUrl && this.s != a.url ? this.s = "" : (this.s = "", chrome.downloads.cancel(a.id), chrome.downloads.erase({
        id: a.id
    }))
};
V.modifyParamsbySite = function(res) {
    // check if its inst
    if(res.origin.indexOf("instagram.com")!=-1){
        var url = new URL(res["2"]);
        var search_params = url.searchParams;
        search_params.delete('bytestart');
        search_params.delete('byteend');
        url.search = search_params.toString();
        res["2"] = url.toString();
    }
};
V.covertJsonString = function(res) {
    console.log("===============",JSON.stringify(res))
    V.modifyParamsbySite(res);
    let resp = {
        method:res["1"],
        source:res["2"],
        title:"", // source subject
        type:res["6"],
        pageTitle:res["4"],
        pageUrl:res["5"],
        mediaInfo:res["9"],
        cookies:res.cookies,
        origin:res.origin,
        referer:res.referer,
        contentType:res["8"],
        fileType:res["12"],
        encode:res.encode,
        agent:navigator.userAgent,
        contentDisposition:res["11"],
        contentLength:res["7"],
    }
    return JSON.stringify(resp)
}
V.sendData = function(a) {
    if (this.D) {
        var b = '';
        let resp = this.covertJsonString(a)
        if (a["3"]) this.G.send(resp), this._data = null;
        else if ("POST" == a["1"] || !this.C || a["7"] && a["8"]) "POST" != a["1"] && this.C && (b += "8:" + a["8"] + "\r\n", b += "7:" + a["7"] + "\r\n"), this.G.send(resp),
        this._data = null;

    }else this._data = a, this.startUpSocket()
}
V.I = async function(a) {
    if (this.D) {
        var b = "1:" + a["1"] + "\r\n";
        if (a.pageUrl) {
            var c = a.pageUrl,
                d = "";
            c &&= c.trim();
            c && (d = (new URL(c)).origin);
            a.origin = d
        }
        if (a.pageUrl) {
            if (c = a.pageUrl) d = c.lastIndexOf("#"), c = 0 > d || d < c.indexOf("?") ? c : c.substr(0, d);
            a.referer = c
        }
        
        "POST" == a["1"] && (a["7"] && (b += "7:" + a["7"] + "\r\n"), a["8"] && (b += "8:" + a["8"] + "\r\n"), b = a.postData ? b + ("__0SVMPostData0__:" + a.postData) : b + "Content-Length: 0\r\n");
        const res = this.covertJsonString(a)
        if (!(118784 < b.length))
            if (a["3"]) this.G.send(res), this._data = null;
            else if ("POST" == a["1"] || !this.C || a["7"] && a["8"]) "POST" != a["1"] && this.C && (b += "8:" + a["8"] + "\r\n", b += "7:" + a["7"] + "\r\n"), this.G.send(res),
            this._data = null;
        else try {
            const f = await fetch(a["2"], {
                method: "HEAD",
                credentials: "include"
            });
            f.ok && (a["8"] = a["8"] || f.headers.get("content-type") || "", a["7"] = a["7"] || f.headers.get("Content-Length") || 0, b += "8:" + a["8"] + "\r\n", b += "7:" + a["7"] + "\r\n", this.G.send(res), this._data = null)
        } catch (f) {}
    } else this._data = a, this.startUpSocket()
};
V.startUpSocket = function() {
    const socket = new WebSocket("ws://localhost:20009/crossdownloader");
    socket.onopen = this.onopen;
    socket.onclose = this.onclose;
    socket.onmessage = this.onmessage;
    socket.onerror = this.onerror;
    this.G = socket
};
V.onopen = function() {
    this.D = !0;
    this._data && this.I(this._data)
};
V.onclose = function() {
    this.D = !1;
    this._data = null
};
V.onmessage = function(a) {
    a = a.data;
    "waiting" == a ? this.C = !0 : "nowaiting" == a ? this.C = !1 : !exists(a, "Version") && N(a, "ShowPanelChrome") && (a = "1" == a.split("=")[1], a != this.F && (this.F = a, chrome.storage.local.set({
        ka: a ? 1 : -1
    }, function() {}), this.ga([13, a])))
};
V.onerror = function() {
    this.D = !1;
    if (this._data) {
        var a = this;
        chrome.tabs.query({
            currentWindow: !0,
            active: !0
        }, function(b) {
            b && b.length && (b = a.h[[b[0].id, 0]]) && b.postMessage([15])
        })
    }
    this._data = null
};
V.cookieHandler = function(a) {
    if (this._data) {
      // Create an empty string to store the cookies.
      let cookies = "";

      // If the `a` array is not empty, iterate over it and add each cookie to the `cookies` string.
      if (a && a.length > 0) {
        for (let c = 0; c < a.length; c++) {
          cookies += a[c].name + "=" + a[c].value + (c < a.length - 1 ? "; " : "");
        }
      }

      // Trim the whitespace from the `cookies` string.
      cookies = cookies.trim();

      // Set the `cookies` property on the `this._data` object.
      this._data.cookies = cookies;

      // Call the `I()` function with the `this._data` object as an argument.
      this.I(this._data);
    }
};
V.menuClicked = function(a, b) {
    var schema = getSchema(a.linkUrl||a.srcUrl), c;
    !schema || "ftp" != schema && "http" != schema && "https" != schema || "ftp" == schema && !getFilename(a.linkUrl) 
    || ( c = new DataConstuctor, 
        c["2"] = a.linkUrl || a.srcUrl, 
        c.pageUrl = a.pageUrl, 
        c["4"] = b && b.title || "", 
        b && b.url && (c["5"] = b.url), 
        c["12"] = getFileType(getFilename(c["2"]))||"html",
        !c["5"] && (c["5"] = a.pageUrl), 
        this._data = c, chrome.cookies.getAll({
        url: c["2"]
    }, this.cookieHandler))
};
V.getDownloadType = (schema, data)=>{
    if(/(ftp|http)/i.exec(schema)){
        return getFileType(getFilename(c["2"]))||"html"
    }

};
function W(a) {
    this.h = a
}
var X = W.prototype;
X.j = function(a) {
    var b = "";
    if (!a) return b;
    if ((a = a.split(",")) && a.length)
        for (var c = 0; c < a.length; c++) {
            var d = a[c].split("=");
            d && 2 == d.length && ("BANDWIDTH" == d[0].toString().trim() && (b += parseInt(parseInt(d[1]) / 1024) + " Kbps "), "RESOLUTION" == d[0].toString().trim() && (b += d[1] + " "))
        }
    return b.trim()
};
// handle "m3u8"
X.i = function(a, b) {
    var c = [],
        d = 0,
        e = "",
        f = this;
    b = b.split(/[\r\n]+/);
    if (0 != b.length && "#EXTM3U" == b[0].trim()) {
        for (var g = !1, m = !1, t = !1, n = "", p = RegExp("^#(EXT[^\\s:]+)(?::(.*))"), B = 1; B < b.length; B++) {
            var k = b[B].trim();
            k && ("#" == k[0] ? 0 == k.indexOf("#EXT") && (k = p.exec(k)) && (g || (g = "EXTINF" == k[1]) && (n = k[2]), m || (m = "EXT-X-STREAM-INF" == k[1]) && (n = k[2]), t ||= "EXT-X-BYTERANGE" == k[1]) : (g && (d += parseFloat(n), g = !1), m && (c.push({
                2: (new URL(k, a["2"])).href,
                tags: n
            }), m = !1), t && !e && (e = (new URL(k, a["2"])).href)))
        }
        if (e) {
            b = "";
            d && (60 < d &&
                (b += parseInt(d / 60) + " min "), b += parseInt(d % 60) && parseInt(d % 60) + " sec");
            var l = {
                6: "media",
                fEx: "ts",
                4: "TS | " + b,
                fDu: b
            };
            l = M(l, {
                1: a["1"],
                2: e,
                tabId: a.tabId,
                frameId: a.frameId,
                fS: a["7"],
                fileName: a.fileName
            });
            Y(a, l);
            "POST" == l["1"] && S(a, l);
            setTimeout(function() {
                f.h.add(l)
            }, 1000)
        } else c.length ? setTimeout(function() {
            for (var w = 0; w < c.length; w++) f.h.add(M({
                tabId: a.tabId,
                frameId: a.frameId
            }, {
                1: "GET",
                2: c[w]["2"],
                6: "hls",
                fEx: "ts",
                4: "TS | " + f.j(c[w].tags)
            }))
        }, 1000) : 0 < d && (b = "", 60 < d && (b += parseInt(d / 60) + " min "), b += parseInt(d %
            60) && parseInt(d % 60) + " sec", l = {
            6: "hls",
            fEx: "ts",
            4: "TS | " + b,
            fDu: b
        }, l = M(l, {
            1: a["1"],
            2: a["2"],
            tabId: a.tabId,
            frameId: a.frameId,
            fS: a["7"],
            fileName: a.fileName
        }), Y(a, l), "POST" == l["1"] && S(a, l), setTimeout(function() {
            f.h.add(l)
        }, 1000))
    }
};
V.stringToID = function(str) {
    let hash = 0;
    for (let i = 0; i < str.length; i++) {
        hash += str.charCodeAt(i);
    }
    return hash
};
V.add = function(mediaObj) {
    var b = this.h[[mediaObj.tabId, mediaObj.frameId]];
    if (!b && (b = this.h[[mediaObj.tabId, 0]], !b)) return;
    var c = mediaObj["2"]
    
    mediaObj.id = this.stringToID(c);
    b.postMessage([1, mediaObj, b["2"]])
};
V.onCompleted = function(a) {
    delete this.l[a.requestId]
};

function fa(a, b) {
    if (!a) return null;
    var c = a.raw;
    if (c) {
        a = "";
        for (b = 0; b < c.length; b++) {
            var d = c[b].bytes;
            if (!d) return null;
            d = new Uint8Array(d);
            for (var e = d.length, f = 0; f < e; f++) a += String.fromCharCode(d[f])
        }
        return a
    }
    c = a.formData;
    if (!c) return null;
    e = getContentType(b);
    a = [];
    e &&= e.toLowerCase();
    if ("application/x-www-form-urlencoded" == e) {
        for (d in c)
            for (e = c[d], d = d.split(" ").map(encodeURIComponent).join("+"), b = 0; b < e.length; b++) a.length && a.push("&"), a.push(d, "=", e[b].split(" ").map(encodeURIComponent).join("+"));
        return a.join("")
    }
    if ("multipart/form-data" ==
        e) {
        (f = Z(b, "boundary")) || (f = "----WebKitFormBoundary" + Math.random().toString(36).substr(2));
        for (d in c)
            for (e = c[d], b = 0; b < e.length; b++) a.push("--", f, '\r\nContent-Disposition: form-data; name="', d, '"\r\n\r\n', e[b], "\r\n");
        a.push("--", f, "--\r\n");
        return a.join("")
    }
    return null
}
V.onHeadersReceived = function(a) {
    var info, c = a.requestId,
        d = this;
    if (info = this.l[c]) {
        var e = a.url,
            f = a.type,
            g = 0 <= y.indexOf(f),
            m = a.method.toUpperCase(),
            t = getSchema(e);
            isYoutube = a.url.indexOf("googlevideo") > -1
        if (!t || "http" != t && "https" != t || "GET" != m && "POST" != m) 
            delete this.l[c];
        else {
            info.responseHeaders = a.responseHeaders;
            var n = L(info.responseHeaders, "Content-Type"),
                p = getContentType(n).toLowerCase();
            if ("image" == f && p && N(p.toLowerCase(), "image/")) 
                delete this.l[c];
            else {
                var B = L(info.responseHeaders, "Content-Disposition"),
                    k = "attachment" == getContentType(B).toLowerCase();
                a = parseInt(a.statusLine.split(" ", 2).pop()) || 0;
                info.ha = 0 <= aa.indexOf(a);
                if (!info.ha) {
                    if (200 == a ||
                        206 == a) {
                        a = L(info.responseHeaders, "Content-Length");
                        var l = L(info.responseHeaders, "Content-Range"),
                            w = null;
                        l && (l = q.exec(l)) && (a = l[1]);
                        a && (w = parseInt(a));
                        if (0 !== w)
                            info["2"] = e;
                            info["8"] = n;
                            info["7"] = w;
                            info.encode = L(info.responseHeaders, "Accept-Encoding");
                            info.type = f;
                            info.protocol = t;
                            info["1"] = m;
                            info.R = O(f, "_frame");

                            // 使用URL构造一个新的URL对象，并获取hostname和pathname
                            const urlObj = new URL(e);
                            const hostname = urlObj.hostname;
                            const pathname = urlObj.pathname;
                            //search urlparams
                            const params = new URLSearchParams(urlObj.search);

                            // 解析pathname获取文件名（去掉后缀和查询参数）
                            let fileName = pathname.split("/").pop().trim();
                            if (fileName) {
                              fileName = fileName.split("?").shift().trim();
                            }
                            info.o = fileName || "";

                            // 获取文件类型
                            info.u = getFileType(info.o);
                            info.K = Z(B, "filename") || Z(n, "name");
                            info.P = info.K && getFileType(info.K) || "";

                            // 获取文件类型的主类型
                            let filetype = p ? MimeTypesMapping[p] : false;
                            if(isYoutube&&params.get("mime")){
                                filetype = params.get("mime").split('/').pop()
                            }
                            info.suffix = (filetype ? filetype.split("|").shift() : "").toLowerCase();
                            
                            // 获取文件类型
                            info.filetype = info.suffix || info.P || info.u || "";

                            // 获取文件名
                            info.fileName = info.K || info.o || "";
                            if (info.fileName) {
                              const lastDotIndex = info.fileName.lastIndexOf(".");
                              if (lastDotIndex !== -1) {
                                info.fileName = info.fileName.substr(0, lastDotIndex).trim();
                              }
                            }

                            // 如果有文件名且有文件类型，则加上文件类型后缀
                            if (info.fileName && info.filetype) {
                              info.fileName += "." + info.filetype;
                            }

                            // 判断是否为特定文件类型或情况
                            const isMainFrameWithJS = "main_frame" == info.type && A.test(info.filetype) && !C.test(info.filetype);
                            const isManifOrFavicon = exists(info.o.toLowerCase(), "manif") || exists(info.o.toLowerCase(), "favicon.ico") || exists(info.o.toLowerCase(), "pem.msg");
                            const isWasmOrJson = O(info.o.toLowerCase(), ".wasm") || exists(info.o.toLowerCase(), ".json") || exists(info.suffix.toLowerCase(), "json") || exists(info.P.toLowerCase(), "json");
                            const isOtherWithJS = !(isMainFrameWithJS || "other" == info.type && A.test(info.filetype) || (info.R || !g) && da.test(info.filetype) || (info.R || "other" == info.type) && (k || ba.test(p) || info.filetype && !A.test(info.filetype) && !ca.test(info.filetype)));

                            // 最终的条件判断结果
                            const isGoodCase = isManifOrFavicon || isWasmOrJson || isOtherWithJS;

                            if (!p && info.filetype && (p = getMimeType(info.filetype)), n = isGoodCase){
                                k = "vtt" == info.filetype.toLowerCase() || "vtt" == info.u.toLowerCase() || "srt" == info.filetype.toLowerCase() || "srt" == info.u.toLowerCase();
                                var H = null;
                                /*
                                "m3u8" == info.filetype.toLowerCase() || 
                                "m3u8" == info.u.toLowerCase() ? H = new W(this) : k || 
                                "POST" == info["1"] || 
                                exists(e.toLowerCase(), "vimeo") || 
                                exists(e.toLowerCase(), "youtube") || 
                                exists(e.toLowerCase(), "google") || 
                                "txt" != info.filetype.toLowerCase() && 
                                "js" != info.filetype.toLowerCase() || 
                                "xmlhttprequest" != info.type || 
                                info["7"] && 307200 < info["7"] || 
                                (H = new W(this));

                                */
                                const fileName = info.filetype.toLowerCase();
                                const urlName = info.u.toLowerCase();
                                const eLower = e.toLowerCase();
                                const infoType = info.type;
                                const info7 = info["7"];

                                if (fileName === "m3u8" || urlName === "m3u8") {
                                    H = new W(this);
                                } else if(!k || info["1"] === "POST" || ["vimeo", "youtube", "google"].includes(eLower) || 
                                    !(fileName === "txt" || fileName === "js") || infoType !== "xmlhttprequest" || (info7 && info7 > 307200)) {
                                        // Nothing happens here according to the original code.
                                } else {
                                    H = new W(this);
                                }

                                if (H) 
                                    R({
                                    2: info["2"],
                                    S: function(r) {
                                        H.i(M({}, info), r)
                                        }
                                    }, M({}, info));
                                else if (g && "player.vimeo.com" == e && N(f, "/video/") && "application/json" == p) 
                                    R({
                                    2: info["2"],
                                    S: function(r) {
                                        var x = null;
                                        try {
                                            x = JSON.parse(r)
                                        } catch (D) {}
                                        if (x) {
                                            var I = x.request.files.progressive;
                                            I && setTimeout(function() {
                                                for (var D = 0; D < I.length; D++) d.add({
                                                    1: "GET",
                                                    2: I[D].url,
                                                    6: "media",
                                                    tabId: info.tabId,
                                                    frameId: info.frameId,
                                                    fEx: "mp4",
                                                    4: "MP4 | " + I[D].quality
                                                })
                                            }, 2500)
                                        }
                                    }
                                    }, info);
                                else if ((g || k) && 
                                    (A.test(info.filetype) || A.test(info.u)) && 
                                    !C.test(info.filetype) && 
                                    (!info["7"] || 204800 < info["7"] || k) && 
                                    !("ASF" == info.filetype && 1024E3 >= info["7"]) && 
                                    "DCLK-AdSvr" != L(info.responseHeaders, "Server")) {
                                    var J = {
                                        2: info["2"],
                                        6: "media",
                                        1: info["1"],
                                        tabId: info.tabId,
                                        frameId: info.frameId,
                                        fEx: A.test(info.filetype) ? info.filetype : info.u,
                                        7: info["7"],
                                        8: info["8"],
                                        fS: info["7"],
                                        fileName: info.fileName,
                                        encode:info.encode,
                                    };
                                    "POST" == J["1"] && S(info, J);
                                    Y(info, J);
                                    setTimeout(function() {
                                        d.add(J)
                                    }, 1000)
                                }
                        }
                        else if (h || !this.v) this.s = "";
                        else {
                            this.s = info["2"];
                            g = d.h[[info.tabId, info.frameId]];
                            p = d.h[[info.tabId, 0]];
                            var u = M(new DataConstuctor, {
                                2: info["2"],
                                1: info["1"],
                                4: p && p["4"] || g && g["4"],
                                5: p && p["2"] || g && g["2"],
                                7: info["7"],
                                8: info["8"],
                                pageUrl: g && g["2"] || info["2"],
                                encode:info.encode
                            });
                            chrome.tabs.query({
                                active: !0,
                                currentWindow: !0
                            }, function(r) {
                                if (r && r.length && (info["2"] == r[0].pendingUrl || 
                                    info["2"] == r[0].url) && !u["5"] && r[0].openerTabId) {
                                    var x =
                                        d.h[[r[0].openerTabId, 0]];
                                    u["5"] = x && x["2"];
                                    u["4"] = x && x["4"];
                                    A.test(info.filetype) && (chrome.tabs.remove(r[0].id), u["6"] = "media")
                                }
                            });
                            "POST" == u["1"] && S(info, u);
                            Y(info, u);
                            d.i = u;
                            chrome.cookies.getAll({
                                url: u["2"]
                            }, d.cookieHandler)
                        }
                    }
                    delete this.l[c]
                }
            }
        }
    }
};

function S(a, b) {
    var c = L(a.m, "Content-Type"),
        d = L(a.m, "Content-Disposition");
    a = fa(a.ja, c);
    if (!a || 1 > a.length) a = null;
    b.postData = a;
    c && (b["10"] = c.trim());
    d && (b["11"] = d.trim())
}

function Y(a, b) {
    if (a.m)
        for (var c = 0; c < a.m.length; c++) N(a.m[c].name.toLowerCase(), "x-") && (b[a.m[c].name] = a.m[c].value)
}
V.onBeforeSendHeaders = function(a) {
    if (!(0 > a.tabId || 0 > a.frameId)) {
        var b = this.l[a.requestId];
        b && (b.m = a.requestHeaders)
    }
};
V.onBeforeRequest = function(a) {
    if (!(0 > a.tabId || 0 > a.frameId))
        if ("ftp" == getSchema(a.url)) {
            if (getFilename(a.url) && !h) {
                var b = new DataConstuctor,
                    c = this.h[[a.tabId, 0]];
                c && c["2"] && (b["5"] = c["2"], b.pageUrl = c["2"]);
                c && c["4"] && (b["4"] = c["4"]);
                b["2"] = a.url;
                this.I(b)
            }
        } else b = a.requestId, c = this.l[b] || {
            id: b,
            2: a.url,
            tabId: a.tabId,
            frameId: a.frameId
        }, "POST" == a.method.toUpperCase() && (c.ja = a.requestBody), this.l[b] = c
};

function Z(a, b) {
    if (!a) return null;
    b = b.toLowerCase();
    a = a.split(";");
    a.shift();
    for (var c = 0; c < a.length; c++) {
        var d = a[c],
            e = d.indexOf("=");
        if (0 < e) {
            var f = d.substr(0, e).trim().toLowerCase(),
                g = "*" == f[f.length - 1];
            g && (f = f.substr(0, f.length - 1).trimRight());
            if (f == b) return a = d.substr(e + 1).trim(), c = a.length - 1, '"' == a[0] && '"' == a[c] && (a = a.substring(1, c)), g && (a = a.split("'", 3).pop()), unescape(a)
        } else if (0 > e && d.trim().toLowerCase() == b) return ""
    }
    return null
}
V.j = function(a) {
    a.addListener.apply(a, Array.prototype.slice.call(arguments).slice(1))
};
V.runtimeConnect = function(a) {
    var b = a.sender.tab;
    if (b && 0 <= b.id) {
        var c = a.sender.frameId,
            d = a.id || this.fa++,
            e = b.id;
        a.id = d;
        a["4"] = b.title;
        a.tabId = e;
        a.frameId = c;
        a.ia = 0 == c;
        a["2"] = a.sender.url || a.ia && b.url || null;
        a.onMessage.addListener(this.aa.bind(this, a));
        a.onDisconnect.addListener(this.$.bind(this, a));
        this.H[d] = a;
        this.h[[e, c]] = a;
        a.postMessage([3, a.id]);
        a.postMessage([13, this.F]);
        a.sender = null
    }
};
V.aa = function(a, b) {
    switch (b[0]) {
        case 2:
            var c = b[2],
                d = b[3];
            (a = this.H[b[1]]) && c && (a["2"] = c);
            a && d && (a["4"] = d);
            break;
        case 4:
            h = b[1];
            break;
        case 6:
            c = b[1];
            a = (a = a.tabId) && this.h[[a, 0]];
            var e = new DataConstuctor;
            e["1"] = c["1"] || "GET";
            e["2"] = c["2"];
            c["3"] && (e["3"] = c["3"]);
            e.pageUrl = b[2];
            e["4"] = b[3] || a && a["4"] || "";
            e["5"] = a && a["2"] || e.pageUrl;
            e["9"] = b[4];
            c["7"] && (e["7"] = c["7"]);
            c["8"] && (e["8"] = c["8"]);
            e["6"] = c["6"] || "media";
            !c.fEx || "vtt" != c.fEx.toLowerCase() && "srt" != c.fEx.toLowerCase() || (e["6"] = "normal");
            c.postData && (e.postData =
                c.postData);
            c["10"] && (e["10"] = c["10"]);
            c["11"] && (e["11"] = c["11"]);
            e["12"] = c.fEx;
            for (d in c) N(d.toLowerCase(), "x-") && (e[d] = c[d]);
            this._data = e;
            chrome.cookies.getAll({
                url: e["2"]
            }, this.cookieHandler)
    }
};
V.$ = function(a) {
    for (var b in this.h) this.h[b] == a && delete this.h[b];
    delete this.H[a.id]
};
V.la = function(a, b) {
    var c = this.h;
    a = a.toString() + ",";
    for (var d in c) N(d, a) && c[d].postMessage(b)
};
V.ga = function(a) {
    var b = this.h,
        c;
    for (c in b) b[c].postMessage(a)
};
new SVMConstructor;
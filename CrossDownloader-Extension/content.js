var h = chrome.runtime.getURL("img/icon16.png"),
    q = chrome.runtime.getURL("img/close16.png"),
    r = {
        "-1": "none",
        1: ""
    };

function getElementById(c) {
    return document.getElementById(c)
}
window.el = getElementById;

function checkFB() {
    var c = document.location.host.toLowerCase(),
        e = c.length - 12;
    return 0 <= e && c.indexOf("facebook.com", e) == e
}
async function fetchVideo(c) {
    try {
        const e = await fetch(c["2"], {
            mode: "no-cors"
        });
        if (e.ok) {
            let a = await e.text();
            (c.oa || function() {})(a)
        }
    } catch (e) {}
}

function B(c) {
    var e = 0,
        a;
    var b = 0;
    for (a = c.length; b < a; b++) {
        var d = c.charCodeAt(b);
        e = (e << 5) - e + d;
        e |= 0
    }
    return e
}

function byteFormat(c) {
    return !c || 0 > c ? " " : 1E3 > c ? c + " Bytes" : 1E6 > c ? (c / 1024).toFixed(1) + " KB" : 1E9 > c ? (c / 1048576).toFixed(2) + " MB" : (c / 1073741824).toFixed(3) + " GB"
}

function getPos(c) {
    if (!c) return {
        left: 0,
        top: 0
    };
    try {
        var e = c.getBoundingClientRect();
        return e ? {
            left: Math.round(e.left + window.pageXOffset),
            top: Math.round(e.top + window.pageYOffset)
        } : {
            left: 0,
            top: 0
        }
    } catch (a) {
        return {
            left: 0,
            top: 0
        }
    }
}

function typeInfo(c) {
    return c ? !c.fEx || "VTT" != c.fEx.toUpperCase() && "SRT" != c.fEx.toUpperCase() ? c["4"] || (c.fEx.toUpperCase() || "MP4") + " |  " + (byteFormat(c.fS) || c.fDu) : c.fEx.toUpperCase() + " Subtitles File " + (c.fS ? byteFormat(c.fS) : " ") : "Media File"
}

function initalSVMCollector(c, e, a) {
    this.v = c;
    c.h[a] = this;
    this.na = a;
    this.H = "svmTable" + a;
    this.L = "svmHCell" + a;
    this.collectorElem = null;
    this.l = e;
    this.i = null;
    this.position = {
        left: 0,
        top: 0
    };
    this.A = -1;
    this.items = [];
    this.O = this.N = 0;
    this.o = this.u = !1
}
var M = initalSVMCollector.prototype;
M.B = function(c) {
    var e = Array.prototype.slice.call(arguments);
    e[2] = e[2].bind(this);
    c.addEventListener.apply(c, e.slice(1))
};
M.s = function() {
    this.collectorElem && (
        this.collectorElem.style.left = this.position.left + "px", 
        this.collectorElem.style.top = this.position.top + "px", 
        this.collectorElem.style.zIndex += 500
        )
};
M.S = function(c) {
    this.I(!0);
    this.v.ha(this.items[c])
};
M.I = function(c) {
    if (!c || -1 != this.A) {
        var e = getElementById(this.H).rows;
        this.A = c ? -1 : -this.A;
        for (c = 1; c < e.length; c++) e[c].style.display = r[this.A]
    }
};
M.F = function(c) {
    var e = this,
        a = getElementById(this.H),
        b = this.v.getVideoObj(this.items[c]);
    var d = a.insertRow(-1);
    d.cssText = "padding:0px;margin:0px;width:100%;line-height:100% !important;height:22px !important";
    d.style.display = r[e.A];
    a = d.insertCell(0);
    a.style.cssText = "letter-spacing:normal;line-height:100% !important;width:100%;height:22px !important;margin:0px;padding:0px;padding-left:5px;vertical-align:middle;color:white !important;cursor:default;background:#000 !important;direction:ltr;text-align:left;font-family:tahoma !important;font-style:normal;font-weight:bold;font-size:10pt !important;border-radius: 4px;";
    b = typeInfo(b);
    if (0 == c && 1 == this.items.length) {
        d = document.createElement("TABLE");
        d.style.cssText = "border-spacing:0px;border-collapse:separate;padding:0px;margin:0px;width:100%;direction:ltr;line-height:100% !important;background:#000";
        var f = d.insertRow(-1);
        f.style.cssText = "padding:0px;margin:0px;line-height:100% !important;height:22px !important";
        var g = f.insertCell(-1);
        g.style.cssText = "padding:0px;margin:0px;width:30px;height:22px !important;text-align:center;vertical-align:middle;line-height:100% !important";
        var k = document.createElement("IMG");
        k.src = h;
        
        g.appendChild(k);
        g = f.insertCell(-1);
        g.id = e.L;
        g.style.cssText = "letter-spacing:normal;padding:0px;margin:0px;vertical-align:middle;color:white !important;cursor:default;direction:ltr;text-align:center;font-family:tahoma !important;font-style:normal;font-weight:bold;font-size:10pt !important;height:22px !important;line-height:100% !important";
        g = f.insertCell(-1);
        g.style.cssText = "padding:0px;margin:0px;width:20px;height:22px !important;text-align:center;vertical-align:middle;line-height:100% !important";
        f = document.createElement("IMG");
        f.src = q;
        f.onclick = function() {
            e.collectorElem.style.display = "none"
        };
        g.appendChild(f);
        a.appendChild(d);
        a.style.paddingLeft = "0px";
        a = getElementById(e.L);
        a.innerText = " " + b;
        a.onmouseover = function() {
            this.style.color = "red"
        };
        a.onmouseout = function() {
            this.style.color = "white"
        };
        a.onclick = function() {
            0 == e.o && e.S(0);
            e.o = !1
        }
    } else a.innerText =
        " " + (c + 1).toString() + "- " + b, a.onmouseover = function() {
            this.style.background = "#000";
            this.style.color = "red"
        }, a.onmouseout = function() {
            this.style.background = "#000";
            this.style.color = "white"
        }, d.onmousedown = function() {
            e.S(c)
        }
};
M.findClosestHeading = function(videoElement) {
  const minTextLength = 20; // Minimum length of text content to be considered a heading
  const maxTextLength = 100; // Maximum length of text content to be considered a heading

  let currentElement = videoElement;
  while (currentElement.parentElement) {
    currentElement = currentElement.parentElement;
    const textElements = currentElement.querySelectorAll('*:not(script):not(style)'); // Select all elements except script and style tags

    for (const textElement of textElements) {
      const textContent = textElement.textContent.trim();
      if (textContent.length >= minTextLength && textContent.length <= maxTextLength) {
        return textContent;
      }
    }
  }
  return 'No heading found';
};
//create collector and items and bind event
M.buildCollector = function(c) {
    var e = this,
        a = this.v.getVideoObj(c), //video object
        b = null,
        //check if video elem exists, use absolute or fixed
        d = this.l ? "absolute" : "fixed";
    this.l && !this.u && (b = getPos(this.l));
    b && (this.position = {
        left: Math.max(0, b.left - 1),
        top: Math.max(0, b.top - 19 - 4) // - 19 - 4
    });
    //create collectorElement
    if(this.collectorElem){
      this.s()  
    } else {
        this.collectorElem = document.createElement("SVM-downloader"), 
        this.collectorElem.style.cssText = "padding:0px;margin:0px;position:" + d + ";z-index:100000000;width:230px;left:" + this.position.left + "px;top:" + this.position.top + "px;direction:ltr;text-align:center;line-height:100% !important;", 
        this.collectorElem.id = "svm" + this.na, 
        this.collectorElem.style.display = this.v.C ? "" : "none", 
        document.body.appendChild(this.collectorElem), 
        b = document.createElement("TABLE"), 
        b.id = this.H, 
        b.style.cssText = "border-spacing:0px;border-collapse:separate;padding:0px;margin:0px;line-height:100% !important;direction:ltr;width:100%;", 
        this.collectorElem.appendChild(b), 
        this.B(e.collectorElem, "mousemove", e.mousemove), 
        this.B(e.collectorElem, "mousedown", e.mousedown), 
        this.B(e.collectorElem, "mouseup", e.mouseup), 
        this.B(e.collectorElem, "mouseout", e.mouseout), 
        this.B(e.collectorElem, "mouseover", e.mouseover), 
        this.i = setTimeout(function() {
                e.i = null;
                e.collectorElem.style.opacity = .45
            },
            3E4);
    }
    if (!(-1 < this.items.indexOf(c) && "hls" != a["6"])) {
        a = typeInfo(a);
        if (this.items.length && (" MP4 File HQ" == a || " MP4 File LQ" == a))
            for (b = 0; b < this.items.length; b++)
                if (a == typeInfo(this.v.getVideoObj(this.items[b]))) {
                    this.items[b] = c;
                    this.collectorElem.style.display = this.v.C ? "" : "none";
                    this.s();
                    return
                } this.items.push(c);
        c = this.items.length - 1;
        a = getElementById(e.H).rows;
        var f;
        c && (f = getElementById(e.L));
        0 == c ? this.F(0) : 1 == c ? (this.F(0), this.F(1), f.onclick = function(g) {
            g.stopPropagation();
            g.preventDefault();
            0 == e.o && e.I();
            e.o = !1
        }, f.innerText = " 2 Files") : (this.F(c), f.innerText =
            " " + (c + 1).toString() + " Files");
        a[0].style.display = ""
    }
};
M.mousedown = function(c) {
    0 == c.button && (this.u = !0, this.o = !1, this.N = c.clientX, this.O = c.clientY, c.stopPropagation(), c.preventDefault())
};
M.mousemove = function(c) {
    if (this.u) {
        this.I(!0);
        var e = c.clientX - this.N,
            a = c.clientY - this.O;
        !this.o && (this.o = e || a);
        this.position.left += e;
        this.position.top += a;
        this.N = c.clientX;
        this.O = c.clientY;
        this.s()
    } else this.o = !1
};
M.mouseout = function() {
    this.u = !1;
    this.i && (clearTimeout(this.i), this.i = null);
    var c = this;
    this.i = setTimeout(function() {
        c.collectorElem.style.opacity = .45;
        c.i = null
    }, 15E3)
};
M.mouseover = function() {
    this.u = !1;
    this.i && (clearTimeout(this.i), this.i = null);
    this.collectorElem.style.opacity = 1
};
M.mouseup = function() {
    this.u = !1
};
if (!window.m) {
    var SVMBuilder = function() {
        this.Y = null;
        this.D = {};
        this.h = {};
        this.M = !1;
        this.U = this.T = -1;
        this.Counter = 1;
        this.j = null;
        this.C = !0;
        this.da = [];
        this.port = chrome.runtime.connect({
            name: "svm"
        });
        this.aa = Math.ceil(2E6 * Math.random());
        this.port.onMessage.addListener(this.eventHandler.bind(this));
        this.port.onDisconnect.addListener(this.W.bind(this));
        if (checkFB()) {
            var a = this;
            this.la = new window.MutationObserver(function(b) {
                b.forEach(function(d) {
                    a.fbParser(d.target)
                })
            });
            this.la.observe(document, {
                childList: !0,
                subtree: !0
            })
        }
        this.m(window, "keydown", this.keyupdown, !0);
        this.m(window, "keyup", this.keyupdown, !0);
        this.m(window, "mouseup", this.mouseup, !0);
        this.m(window, "resize", this.resize);
        this.m(document, "DOMContentLoaded", this.ContentLoaded);
        this.m(document, "click", this.click)
    };
    window.m = !0;
    M = SVMBuilder.prototype;
    M.addFbItem = function(a, b) {
        b.hd && this.addItem({
            id: B(b.hd),
            1: "GET",
            2: b.hd,
            fEx: "mp4",
            4: " MP4 File HQ"
        }, window.location.href, a, !1);
        b.sd && this.addItem({
            id: B(b.sd),
            1: "GET",
            2: b.sd,
            fEx: "mp4",
            4: " MP4 File LQ"
        }, window.location.href, a, !1)
    };
    M.fa = function(a) {
        for (;
            (a = a.parentElement) && 1 > a.querySelectorAll("video").length;);
        if (a) return a.querySelectorAll("video")[0]
    };
    M.getFbVideo = function(a, b) {
        var d = this;
        fetchVideo({
            2: "https://www.facebook.com/video/embed?video_id=" + b,
            oa: function(f) {
                var g = /"sd_src_no_ratelimit":"(.*?)"/.exec(f),
                    k = /"hd_src_no_ratelimit":"(.*?)"/.exec(f);
                g && g.length || (g = /"sd_src":"(.*?)"/.exec(f));
                k && k.length || (k = /"hd_src":"(.*?)"/.exec(f));
                f = {
                    sd: g && g.length ? g[1].replace(/\\/g, "") : "",
                    hd: k && k.length ? k[1].replace(/\\/g, "") : ""
                };
                g = d.fa(a);
                void 0 !== g && d.addFbItem(g, f)
            }
        })
    };
    M.fbParser = function(a) {
        var b = this;
        a = a.querySelectorAll('a[href*="/videos/"]');
        a.length && Array.from(a, function(d) {
            if (!d.getAttribute("SVM_DR")) {
                d.setAttribute("SVM_DR", 1);
                var f = d.href.match(/.*\/videos\/(\d+)\/.*/i);
                f && b.getFbVideo(d, f[1])
            }
        })
    };
    M.getVideoObj = function(a) {
        return this.D[a]
    };
    M.ua = function() {
        this.port.postMessage([2, this.Y, window.location.href, this.getTitle()])
    };
    M.ContentLoaded = function() {
        for (var a = this, b = document.getElementsByTagName("SCRIPT"), d, f, g, k = /"progressive":\s*\[/, l = 0; l < b.length; l++)
            if (d = b[l], 0 <= document.location.host.toLowerCase().indexOf("vimeo") && !d.src && k.test(d.innerText) && (d =
                    d.innerText, f = d.indexOf('"progressive"'), !(0 > f || (g = d.indexOf("]", f), 0 > g)))) {
                d = d.substr(f, g - f + 1);
                f = null;
                try {
                    f = JSON.parse("{" + d + "}")
                } catch (p) {}
                if (f) {
                    var n = f.progressive;
                    n && setTimeout(function() {
                        for (var p = 0; p < n.length; p++) a.addItem({
                            id: B(n[p].url),
                            1: "GET",
                            2: n[p].url,
                            fEx: "mp4",
                            4: "MP4 | " + n[p].quality
                        }, window.location.href, null, !1)
                    }, 2E3);
                    break
                }
            }
    };
    M.Z = function(a) {
        var b = this.h[a];
        if (b) {
            try {
                document.body.removeChild(b.collectorElem), b.i && clearTimeout(b.i)
            } catch (d) {}
            delete this.h[a]
        }
    };
    M.ha = function(a) {
        (a = this.D[a]) && this.port.postMessage([6,
            a, window.location.href, this.getTitle(), typeInfo(a)
        ])
    };
    M.ea = function(a, b) {
        var d = null,
            f = ["VIDEO", "AUDIO", "OBJECT", "EMBED"];
        try {
            var g = document.activeElement,
                k = 0,
                l, n, p = g && 0 <= f.indexOf(g.tagName) ? g : null;
            p ||= (g = document.elementFromPoint(this.T, this.U)) && 0 <= f.indexOf(g.tagName) ? g : null;
            for (var v = 0; v < f.length; v++) {
                for (var E = document.getElementsByTagName(f[v]), z = 0; z < E.length; z++)
                    if (g = E[z], 3 != v || "application/x-shockwave-flash" == g.type.toLowerCase()) {
                        var A = g.src || g.data;
                        if (A && (A == a || A == b)) {
                            var F = g;
                            break
                        }
                        if (p || G) var G =
                            g;
                        else {
                            var w = g.clientWidth,
                                x = g.clientHeight;
                            if (w && x) {
                                var H = window.getComputedStyle(g);
                                if (!H || "hidden" != H.visibility) {
                                    var I = w * x;
                                    x < 1.4 * w && w < 3 * x && I > k && (k = I, l = g);
                                    n ||= g
                                }
                            }
                        }
                    } if (F) break
            }(d = F || p || G || l || n) || (d = document.querySelectorAll("video,audio")[0]);
            if (!d) return null;
            if ("EMBED" == d.tagName && !d.clientWidth && !d.clientHeight) {
                var J = d.parentElement;
                "OBJECT" == J.tagName && (d = J)
            }
            return d
        } catch (O) {
            return null
        }
    };
    M.ga = function(a) {
        try {
            var b = parseInt(a.getAttribute("SVM_ELM"));
            b || (b = this.aa << 10 | this.Counter++, a.setAttribute("SVM_ELM",
                b));
            return b
        } catch (d) {}
    };
    M.getTitle = function() {
        var a = "";
        try {
            a = document.title || document.getElementsByTagName("title")[0].innerText, a = a.trim()
        } catch (b) {}
        return a ? a = a.replace(/[ \t\r\n\u25B6]+/g, " ").trim() : ""
    };
    M.keyupdown = function(a) {
        8 != a.keyCode && 46 != a.keyCode || this.port.postMessage([4, "keydown" == a.type])
    };
    M.mouseup = function(a) {
        0 == a.button && (this.T = a.clientX, this.U = a.clientY)
    };
    M.resize = function() {
        if (!this.M) {
            this.M = !0;
            var a = this;
            window.setTimeout(function() {
                for (var b in a.h) {
                    var d = a.h[b],
                        f = null;
                    d.l && (f = getPos(d.l));
                    if (f) {
                        try {
                            document.body.removeChild(d.collectorElem)
                        } catch (g) {}
                        d.position.left =
                            Math.max(0, f.left - 1);
                        d.position.top = Math.max(0, f.top - 19 - 4);
                        document.body.appendChild(d.collectorElem)
                    }
                    d.s()
                }
                a.M = !1
            }, 500)
        }
    };

    function c(a, b) {
        return 18 > Math.abs(a.left - b.left) && 18 > Math.abs(a.top - b.top)
    }

    function e(a) {
        a = getPos(a.l);
        return !a || 0 > a.left || 0 > a.top
    }
    function forbidden(){
        return RegExp(".*facebook.com$|.*vimeo.com$|.*youtube.com$", "i").test(window.location.host)
    }
    M.addItem = function(a, b, d, f) {
        var g = this,
            k = -1,
            l = null,
            n;
        f = f && forbidden() && !(a.fEx && "VTT" == a.fEx.toUpperCase()) && !(!a.fS || 4194304 < a.fS);
        if (!(0 <= window.location.host.toLowerCase().indexOf("youtube."))) {
            a.id ||
                (a.id = B(a["2"]));
            d ||= this.ea(a["2"], b);
            if (!d)
                for (n in this.h) {
                    l = this.h[n];
                    k = n;
                    break
                }
            if (!d && !l) {
                if (f) return;
                l = new initalSVMCollector(g, null, 0)
            } else if (!l)
                if (k = this.ga(d), l = this.h[k], !l) {
                    if (f) return;
                    l = new initalSVMCollector(g, d, k);
                    b = getPos(d);
                    d = {
                        left: Math.max(0, b.left - 1),
                        top: Math.max(0, b.top - 19 - 4)
                    };
                    for (n in this.h)
                        if (n && n != k && (b = this.h[n], c(d, b.position))) {
                            for (d = 0; d < b.items.length; d++) l.buildCollector(b.items[d]);
                            this.Z(n);
                            break
                        } if (0 != k && this.h[0]) {
                        b = this.h[0];
                        for (d = 0; d < b.items.length; d++) l.buildCollector(b.items[d]);
                        this.Z(0)
                    }
                } else {
                    if (f) {
                        l.s();
                        return
                    }
                }
            else if (f) {
                l.s();
                return
            }
            g.D[a.id] = a;
            l.buildCollector(a.id);
            l.l && e(l) && !this.j && (g.j = setInterval(function() {
                g.ba(l)
            }, 1200))
        }
    };
    M.ba = function(a) {
        if (a && a.l) {
            var b = getPos(a.l);
            b && (a.position = {
                left: Math.max(0, b.left - 1),
                top: Math.max(0, b.top - 19 - 4)
            }, a.s());
            !b || 0 > b.left || 0 > b.top || (clearInterval(this.j), this.j = null)
        } else clearInterval(this.j), this.j = null
    };
    M.eventHandler = function(a) {
        var b = this;
        switch (a[0]) {
            case 1:
                b.addItem(a[1], a[2], null, !0);
                break;
            case 3:
                (a = a[1]) && (b.Y = a);
                b.ua();
                break;
            case 5:
                b.$();
                break;
            case 11:
                b.cleanItems();
                0 <= (new URL(window.location.href)).hostname.toLowerCase().indexOf("youtubex.") ||
                    setTimeout(function() {
                        b.ContentLoaded()
                    }, 2500);
                break;
            case 13:
                a = a[1];
                a != b.C && (b.C = a, b.$());
                break;
            case 15:
                alert("未发现【CrossDownloader】！不要忘记打开哦！")
        }
    };
    M.m = function(a) {
        var b = Array.prototype.slice.call(arguments);
        b[2] = b[2].bind(this);
        this.da.push(b);
        a.addEventListener.apply(a, b.slice(1))
    };
    M.click = function() {
        for (var a in this.h) this.h[a].I(!0)
    };
    M.cleanItems = function() {
        try {
            for (var a in this.h) this.h[a].i && clearTimeout(this.h[a].i), document.body.removeChild(this.h[a].collectorElem)
        } catch (b) {}
        this.h = {};
        this.D = {};
        this.j && clearInterval(this.j);
        this.j = null
    };
    M.$ = function() {
        var a = this.C ? "" : "none";
        try {
            for (var b in this.h) this.h[b].collectorElem.style.display = a
        } catch (d) {}
    };
    M.W = function() {
        this.port = chrome.runtime.connect({
            name: "svm"
        });
        this.port.onMessage.addListener(this.eventHandler.bind(this));
        this.port.onDisconnect.addListener(this.W.bind(this))
    };
    new SVMBuilder
};
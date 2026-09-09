'use strict';
var Clay=require('@rebble/clay'),clay=new Clay(require('./config.json'),null,{autoHandleEvents:false});
var controller=require('./feed').create({Pebble:Pebble,storage:localStorage,Request:XMLHttpRequest,now:Date.now,later:setTimeout,cancel:clearTimeout});
Pebble.addEventListener('ready',controller.ready);
Pebble.addEventListener('appmessage',controller.refresh);
Pebble.addEventListener('showConfiguration',function(){clay.setSettings(controller.settings());Pebble.openURL(clay.generateUrl());});
Pebble.addEventListener('webviewclosed',function(e){if(!e||!e.response)return;try{var r=clay.getSettings(e.response,false),s={};Object.keys(r).forEach(function(k){s[k]=r[k]&&typeof r[k]==='object'?r[k].value:r[k];});controller.configure(s);}catch(ignore){}});

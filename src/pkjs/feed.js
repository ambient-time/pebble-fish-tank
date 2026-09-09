// Copyright 2026 Luke Steuber. MIT License.
'use strict';
var KEY='fish-count.tides.v1',INTERVAL=21600000;
function copy(v){var out={};Object.keys(v).forEach(function(k){out[k]=v[k];});return out;}
function finite(x){return typeof x==='number'&&isFinite(x);}
function normalize(s){if(!s||typeof s!=='object')return null;var station=String(s.STATION||'').trim(),enabled=s.ENABLED===true||s.ENABLED===1?1:0;if(!/^(|[0-9]{7})$/.test(station)||(enabled&&!station))return null;return {ENABLED:enabled,STATION:station};}
function url(s,now){if(!s.ENABLED||!s.STATION)return null;var d=new Date(Math.floor(now/3600)*3600000),date=d.toISOString().slice(0,10).replace(/-/g,'')+' '+d.toISOString().slice(11,16);return 'https://api.tidesandcurrents.noaa.gov/api/prod/datagetter?product=predictions&application=AmbientTimeFishCount&station='+s.STATION+'&begin_date='+encodeURIComponent(date)+'&range=48&datum=MLLW&units=metric&time_zone=gmt&interval=h&format=json';}
function parse(body,s,now){
 var rows=body&&body.predictions;if(!Array.isArray(rows)||rows.length<49||rows.length>100)return null;
 var samples=[];for(var i=0;i<rows.length;i++){var r=rows[i];if(!r||typeof r.t!=='string'||!/^\d{4}-\d{2}-\d{2} \d{2}:00$/.test(r.t)||typeof r.v!=='string'||!/^[-+]?[0-9]+(?:\.[0-9]+)?$/.test(r.v))return null;var at=Date.parse(r.t.replace(' ','T')+':00Z')/1000,value=Number(r.v);if(!finite(at)||!finite(value)||Math.abs(value)>30)return null;if(samples.length&&at-samples[samples.length-1].at!==3600)return null;samples.push({at:at,value:value});}
 var first=samples[0].at;if(first>now||now-first>3600)return null;samples=samples.slice(0,49);
 var low=Math.min.apply(null,samples.map(function(x){return x.value;})),high=Math.max.apply(null,samples.map(function(x){return x.value;}));if(high-low<0.02)return null;
 var series=samples.map(function(x){var n=Math.round((x.value-low)*255/(high-low));return ('0'+n.toString(16)).slice(-2);}).join('');return {AT:first,SERIES:series};
}
exports.normalize=normalize;exports.url=url;exports.parse=parse;
exports.create=function(env){var s=normalize({}),revision=0,cache=null,generation=0,queue=[],sending=false,retry=null,timer=null,xhr=null,last=0;
 try{var saved=JSON.parse(env.storage.getItem(KEY)||'null');if(saved&&normalize(saved.s)&&finite(saved.revision)&&saved.revision%1===0&&saved.revision>=0&&saved.revision<2147483646){s=normalize(saved.s);revision=Number(saved.revision)||0;cache=null;}}catch(ignore){}
 function now(){return Math.floor(env.now()/1000);}
 function save(){try{env.storage.setItem(KEY,JSON.stringify({s:s,revision:revision,cache:cache}));}catch(ignore){}}
 function drain(){if(sending||retry!==null)return;while(queue.length&&queue[0].token!==generation)queue.shift();if(!queue.length)return;var item=queue.shift();sending=true;var finished=false;function done(ok){if(finished)return;finished=true;sending=false;if(!ok&&item.token===generation&&item.attempt<3){retry=env.later(function(){retry=null;if(item.token===generation)queue.unshift(item);drain();},[1000,5000,30000][item.attempt++]);}else drain();}try{env.Pebble.sendAppMessage(item.value,function(){done(true);},function(){done(false);});}catch(ignore){done(false);}}
 function send(v){v.REVISION=revision;queue.push({value:v,token:generation,attempt:0});drain();}
 function config(){send({ENABLED:s.ENABLED});}
 function refresh(){if(timer!==null)env.cancel(timer);timer=null;config();if(s.ENABLED&&cache)send(copy(cache));var u=url(s,now());if(!u)return;timer=env.later(refresh,INTERVAL);if(xhr||(last&&env.now()>=last&&env.now()-last<INTERVAL))return;last=env.now();var token=generation,req=new env.Request();xhr=req;var finished=false;function complete(body){if(finished)return;finished=true;if(token!==generation)return;xhr=null;var value=parse(body,s,now());if(value){cache=value;save();send(copy(value));}}req.onload=function(){var body=null;if(req.status>=200&&req.status<300&&req.responseText.length<500000){try{body=JSON.parse(req.responseText);}catch(ignore){}}complete(body);};req.onerror=req.ontimeout=req.onabort=function(){complete(null);};try{req.open('GET',u,true);req.timeout=20000;req.send();}catch(ignore){complete(null);}}
 function configure(next){var clean=normalize(next);if(!clean)return false;generation++;revision=(revision+1)%2147483646;s=clean;cache=null;last=0;queue=[];if(retry!==null)env.cancel(retry);retry=null;if(timer!==null)env.cancel(timer);timer=null;var old=xhr;xhr=null;if(old)old.abort();save();refresh();return true;}
 return {ready:refresh,refresh:refresh,configure:configure,settings:function(){return JSON.parse(JSON.stringify(s));}};
};

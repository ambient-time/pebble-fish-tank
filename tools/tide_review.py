"""Write a self-contained review of the frozen native tide captures."""
import base64
import hashlib
import json
from pathlib import Path
ROOT=Path(__file__).resolve().parent.parent
OUT=ROOT.parents[2]/'docs/reviews/fish-count-tides-20260909.html'
def main():
    version=json.loads((ROOT/'package.json').read_text())['version']
    pbw=ROOT/'release'/f'fish-tank-{version}.pbw'
    digest=hashlib.sha256(pbw.read_bytes()).hexdigest()
    cards=[]
    for target in ['basalt','diorite','emery','flint','chalk','gabbro']:
        report=json.loads((ROOT/'evidence'/f'{target}-report.json').read_text())
        assert report['pbwSHA256']==digest and report['tideLowHigh'] and report['tideExpiredAndDisabled']
        images=[]
        for state,title in [('low','Low tide'),('high','High tide'),('expired','Expired forecast')]:
            path=ROOT/'evidence'/f'{target}-tide-{state}.png'
            assert str(path.relative_to(ROOT)) in report['frames']
            data=base64.b64encode(path.read_bytes()).decode()
            images.append(f'<figure><img src="data:image/png;base64,{data}" alt="Fish Count on {target}: {title}"><figcaption>{title}</figcaption></figure>')
        cards.append(f'<section><h2>{target.title()}</h2><div class="frames">'+''.join(images)+'</div></section>')
    OUT.write_text('''<!doctype html><html lang="en"><meta charset="utf-8"><meta name="viewport" content="width=device-width,initial-scale=1"><title>Fish Count — tide waterline</title><style>body{font:17px/1.55 system-ui;background:#08151b;color:#eef8fa;max-width:1120px;margin:auto;padding:24px}h1{line-height:1.2}a{color:#93e7f4}section{border-top:1px solid #36515a;margin-top:28px}.frames{display:flex;flex-wrap:wrap;gap:20px}figure{margin:0 0 20px}img{image-rendering:pixelated;max-width:100%;height:auto}figcaption{font-size:14px}code{overflow-wrap:anywhere;font-size:12px}p{max-width:850px}</style><h1>Fish Count: the tank follows the tide</h1><p>Water rises and falls with NOAA tide predictions for the station selected in settings. The fish stay below the surface. The existing fish, kelp and nugget clock remains.</p><p>Optional feed, off by default. The low and high predictions in the cached 48-hour window set the tank's range; hourly samples are interpolated on the watch. With no valid forecast, the surface line disappears. No automatic location.</p><p>Native emulator captures below use controlled low/high/expired samples in the production package. A live NOAA response passed the production parser separately. Six target builds, clock rollovers, animation cadence and malformed-packet checks passed. Phone delivery and physical-watch tests remain open. Not published.</p>'''+f'<p><a href="../../appstores/pebble/fish-tank/release/fish-tank-{version}.pbw">Download version {version}</a> · <a href="../../appstores/pebble/fish-tank/release/fish-tank-{version}-source.zip">Source archive</a> · <a href="https://api.tidesandcurrents.noaa.gov/api/prod/">NOAA source documentation</a></p><p>Package SHA-256: <code>{digest}</code></p>'+''.join(cards)+'</html>')
    print(OUT)
if __name__=='__main__':main()

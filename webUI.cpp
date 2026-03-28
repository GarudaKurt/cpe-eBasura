#include <Arduino.h>
#include "webUI.h"

// Constructor
WEBUI::WEBUI() {}


// HTML Builder
String WEBUI::buildHTML() {

return R"rawhtml(
<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>TrashBin Monitor</title>
<style>
*{box-sizing:border-box;margin:0;padding:0;}
body{font-family:system-ui,sans-serif;background:#f3f4f2;color:#1a1a1a;height:100vh;overflow:hidden;display:flex;flex-direction:column;}
.hdr{display:flex;align-items:center;gap:10px;padding:11px 18px;background:#fff;border-bottom:0.5px solid #e0e0d8;}
.hdr-title{font-size:15px;font-weight:500;}
.hdr-sub{font-size:12px;color:#888;margin-left:4px;}
.badge{font-size:11px;padding:2px 8px;border-radius:20px;font-weight:500;}
.badge-ok{background:#e8f5e9;color:#2e7d32;}
.badge-err{background:#ffebee;color:#c62828;animation:pulse 1s infinite;}
@keyframes pulse{0%,100%{opacity:1}50%{opacity:.4}}
.stats-row{display:grid;grid-template-columns:repeat(5,minmax(0,1fr));gap:10px;padding:10px 18px;background:#fff;border-bottom:0.5px solid #e0e0d8;}
.scard{background:#f3f4f2;border-radius:8px;padding:9px 12px;}
.sval{font-size:22px;font-weight:500;line-height:1;}
.slbl{font-size:11px;color:#888;margin-top:3px;}
.body{display:flex;flex:1;overflow:hidden;}
.map-wrap{flex:1;padding:12px 16px;overflow:hidden;}
.map-lbl{font-size:11px;color:#888;margin-bottom:7px;font-weight:500;letter-spacing:.04em;text-transform:uppercase;}
.map-area{background:#fff;border:0.5px solid #e0e0d8;border-radius:12px;overflow:hidden;height:calc(100% - 26px);position:relative;}
.map-svg{width:100%;height:100%;}
.sidebar{width:252px;border-left:0.5px solid #e0e0d8;overflow-y:auto;background:#fff;}
.side-hdr{font-size:11px;color:#888;padding:9px 14px;border-bottom:0.5px solid #e0e0d8;font-weight:500;letter-spacing:.04em;text-transform:uppercase;position:sticky;top:0;background:#fff;z-index:2;}
.brow{display:flex;align-items:center;gap:8px;padding:8px 14px;border-bottom:0.5px solid #f0f0e8;cursor:pointer;transition:background .1s;}
.brow:hover,.brow.hl{background:#f8f8f4;}
.bdot{width:8px;height:8px;border-radius:50%;flex-shrink:0;}
.bname{font-size:12px;font-weight:500;flex:1;}
.bstatus{font-size:11px;color:#aaa;}
.bpct{font-size:12px;font-weight:500;min-width:33px;text-align:right;}
.bbar-bg{width:48px;height:3px;background:#eee;border-radius:2px;overflow:hidden;margin-top:3px;}
.bbar-fill{height:100%;border-radius:2px;}
.tip{position:absolute;background:#fff;border:0.5px solid #ccc;border-radius:8px;padding:8px 11px;font-size:11px;color:#222;pointer-events:none;z-index:50;white-space:nowrap;opacity:0;transition:opacity .1s;}
.tip.show{opacity:1;}
</style>
</head>
<body>

<div class="hdr">
  <svg width="18" height="18" viewBox="0 0 18 18" fill="none">
    <rect x="3" y="5" width="12" height="11" rx="1.5" stroke="currentColor" stroke-width="1.2" fill="none"/>
    <path d="M6 5V4a1 1 0 0 1 1-1h4a1 1 0 0 1 1 1v1" stroke="currentColor" stroke-width="1.2" fill="none"/>
    <line x1="2" y1="5" x2="16" y2="5" stroke="currentColor" stroke-width="1.2" stroke-linecap="round"/>
  </svg>
  <span class="hdr-title">TrashBin Monitor</span>
  <span class="hdr-sub">Outdoor Area &middot; 16 bins</span>
  <span class="badge badge-ok" style="margin-left:auto" id="conn-badge">Live</span>
  <span style="font-size:11px;color:#aaa;margin-left:8px" id="clk"></span>
</div>

<div class="stats-row">
  <div class="scard"><div class="sval" id="st-total">16</div><div class="slbl">Total bins</div></div>
  <div class="scard"><div class="sval" id="st-ok"   style="color:#2e7d32">--</div><div class="slbl">Online</div></div>
  <div class="scard"><div class="sval" id="st-full" style="color:#c62828">--</div><div class="slbl">Full &ge;90%</div></div>
  <div class="scard"><div class="sval" id="st-warn" style="color:#e65100">--</div><div class="slbl">Warning &ge;70%</div></div>
  <div class="scard"><div class="sval" id="st-avg">--%</div><div class="slbl">Avg fill</div></div>
</div>

<div class="body">
  <div class="map-wrap">
    <div class="map-lbl">Outdoor floor plan</div>
    <div class="map-area">
      <svg class="map-svg" viewBox="0 0 760 400" xmlns="http://www.w3.org/2000/svg">
        <rect width="760" height="400" fill="#f8f8f4"/>
        <rect x="0" y="150" width="760" height="100" fill="#eeeee8"/>
        <line x1="0" y1="200" x2="760" y2="200" stroke="#ccc" stroke-width="0.5" stroke-dasharray="10 7"/>
        <text x="10" y="14" font-family="system-ui,sans-serif" font-size="9" fill="#bbb" letter-spacing="1">ZONE A - NORTH</text>
        <text x="10" y="393" font-family="system-ui,sans-serif" font-size="9" fill="#bbb" letter-spacing="1">ZONE B - SOUTH</text>
        <text x="380" y="203" font-family="system-ui,sans-serif" font-size="9" fill="#bbb" text-anchor="middle" dominant-baseline="middle">Pathway</text>
        <circle cx="40"  cy="75"  r="18" fill="none" stroke="#ddd" stroke-width="0.5"/>
        <circle cx="720" cy="80"  r="14" fill="none" stroke="#ddd" stroke-width="0.5"/>
        <circle cx="50"  cy="330" r="16" fill="none" stroke="#ddd" stroke-width="0.5"/>
        <circle cx="715" cy="325" r="12" fill="none" stroke="#ddd" stroke-width="0.5"/>
        <g id="mbins"></g>
      </svg>
      <div class="tip" id="tip"></div>
    </div>
  </div>

  <div class="sidebar">
    <div class="side-hdr">Bin status list</div>
    <div id="blist"></div>
  </div>
</div>

<script>
const REFRESH_MS = 2000;

function statusColor(f) {
  if (f >= 90) return '#c62828';
  if (f >= 70) return '#e65100';
  return '#2e7d32';
}
function statusLabel(f) {
  if (f >= 90) return 'Full';
  if (f >= 70) return 'Warning';
  return 'OK';
}

function renderMap(bins) {
  const g  = document.getElementById('mbins');
  g.innerHTML = '';
  const NS = 'http://www.w3.org/2000/svg';
  bins.forEach(b => {
    const cx = b.x * 7.6;
    const cy = b.y * 4.0;
    const col = statusColor(b.fill);
    const r = 15, arcR = 10;
    const circ = 2 * Math.PI * arcR;
    const dash = (b.fill / 100) * circ;

    const bg = document.createElementNS(NS, 'circle');
    bg.setAttribute('cx', cx); bg.setAttribute('cy', cy); bg.setAttribute('r', r);
    bg.setAttribute('fill', '#fff'); bg.setAttribute('stroke', '#ddd'); bg.setAttribute('stroke-width', '0.5');
    g.appendChild(bg);

    const arc = document.createElementNS(NS, 'circle');
    arc.setAttribute('cx', cx); arc.setAttribute('cy', cy); arc.setAttribute('r', arcR);
    arc.setAttribute('fill', 'none'); arc.setAttribute('stroke', col); arc.setAttribute('stroke-width', '4');
    arc.setAttribute('stroke-dasharray', `${dash} ${circ}`);
    arc.setAttribute('stroke-dashoffset', `${circ * 0.25}`);
    arc.setAttribute('transform', `rotate(-90 ${cx} ${cy})`);
    g.appendChild(arc);

    const pct = document.createElementNS(NS, 'text');
    pct.setAttribute('x', cx); pct.setAttribute('y', cy + 3.5);
    pct.setAttribute('text-anchor', 'middle');
    pct.setAttribute('font-family', 'system-ui,sans-serif');
    pct.setAttribute('font-size', '8'); pct.setAttribute('font-weight', '500');
    pct.setAttribute('fill', col);
    pct.textContent = b.fill + '%';
    g.appendChild(pct);

    const lbl = document.createElementNS(NS, 'text');
    lbl.setAttribute('x', cx); lbl.setAttribute('y', cy + r + 10);
    lbl.setAttribute('text-anchor', 'middle');
    lbl.setAttribute('font-family', 'system-ui,sans-serif');
    lbl.setAttribute('font-size', '8.5');
    lbl.setAttribute('fill', '#aaa');
    lbl.textContent = b.name;
    g.appendChild(lbl);

    const hit = document.createElementNS(NS, 'circle');
    hit.setAttribute('cx', cx); hit.setAttribute('cy', cy);
    hit.setAttribute('r', r + 8); hit.setAttribute('fill', 'transparent');
    hit.style.cursor = 'pointer';
    hit.addEventListener('mouseenter', e => showTip(e, b));
    hit.addEventListener('mouseleave', () => document.getElementById('tip').classList.remove('show'));
    hit.addEventListener('click', () => hlRow(b.id));
    g.appendChild(hit);
  });
}

function showTip(e, b) {
  const tip  = document.getElementById('tip');
  const rect = document.querySelector('.map-area').getBoundingClientRect();
  const col  = statusColor(b.fill);
  tip.innerHTML = `<b>${b.name}</b><br>Fill: <span style="color:${col};font-weight:500">${b.fill}%</span> - ${statusLabel(b.fill)}<br>Dist: ${b.dist} mm`;
  tip.style.left = (e.clientX - rect.left + 14) + 'px';
  tip.style.top  = (e.clientY - rect.top  - 10) + 'px';
  tip.classList.add('show');
}

function renderList(bins) {
  const el = document.getElementById('blist');
  el.innerHTML = '';
  bins.forEach(b => {
    const col = statusColor(b.fill);
    const row = document.createElement('div');
    row.className = 'brow'; row.id = 'row-' + b.id;
    row.innerHTML = `
      <div class="bdot" style="background:${col}"></div>
      <div style="flex:1;min-width:0">
        <div class="bname">${b.name}</div>
        <div class="bstatus">${statusLabel(b.fill)}</div>
      </div>
      <div>
        <div class="bpct" style="color:${col}">${b.fill}%</div>
        <div class="bbar-bg"><div class="bbar-fill" style="width:${b.fill}%;background:${col}"></div></div>
      </div>`;
    row.addEventListener('click', () => hlRow(b.id));
    el.appendChild(row);
  });
}

function hlRow(id) {
  document.querySelectorAll('.brow').forEach(r => r.classList.remove('hl'));
  const r = document.getElementById('row-' + id);
  if (r) { r.classList.add('hl'); r.scrollIntoView({ behavior: 'smooth', block: 'nearest' }); }
}

function renderStats(bins) {
  const ok   = bins.filter(b => b.ok).length;
  const full = bins.filter(b => b.full).length;
  const warn = bins.filter(b => b.fill >= 70 && b.fill < 90 && b.ok).length;
  const avg  = ok ? Math.round(bins.filter(b => b.ok).reduce((s, b) => s + b.fill, 0) / ok) : 0;
  document.getElementById('st-ok').textContent   = ok;
  document.getElementById('st-full').textContent = full;
  document.getElementById('st-warn').textContent = warn;
  document.getElementById('st-avg').textContent  = avg + '%';
}

function tick() {
  document.getElementById('clk').textContent = new Date().toLocaleTimeString();
}

async function fetchBins() {
  try {
    const res  = await fetch('/api/bins');
    const data = await res.json();
    renderMap(data.bins);
    renderList(data.bins);
    renderStats(data.bins);
    document.getElementById('conn-badge').className   = 'badge badge-ok';
    document.getElementById('conn-badge').textContent = 'Live';
  } catch (e) {
    document.getElementById('conn-badge').className   = 'badge badge-err';
    document.getElementById('conn-badge').textContent = 'Offline';
  }
}

setInterval(fetchBins, REFRESH_MS);
setInterval(tick, 1000);
fetchBins();
tick();
</script>
</body>
</html>
)rawhtml";

}
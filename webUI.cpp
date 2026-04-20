#include <Arduino.h>
#include "webUI.h"

WEBUI::WEBUI() {}

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

/* ── Header ───────────────────────────────── */
.hdr{display:flex;align-items:center;gap:10px;padding:11px 18px;background:#fff;border-bottom:0.5px solid #e0e0d8;}
.hdr-title{font-size:15px;font-weight:500;}
.hdr-sub{font-size:12px;color:#888;margin-left:4px;}
.badge{font-size:11px;padding:2px 8px;border-radius:20px;font-weight:500;}
.badge-ok{background:#e8f5e9;color:#2e7d32;}
.badge-err{background:#ffebee;color:#c62828;animation:pulse 1s infinite;}
@keyframes pulse{0%,100%{opacity:1}50%{opacity:.4}}

/* ── Schedule button ──────────────────────── */
.btn-sched{display:flex;align-items:center;gap:5px;padding:5px 11px;border-radius:7px;border:0.5px solid #d0d0c8;background:#fff;font-size:12px;font-weight:500;color:#444;cursor:pointer;transition:background .12s;}
.btn-sched:hover{background:#f3f4f2;}
.btn-sched svg{flex-shrink:0;}

/* ── Stats ────────────────────────────────── */
.stats-row{display:grid;grid-template-columns:repeat(5,minmax(0,1fr));gap:10px;padding:10px 18px;background:#fff;border-bottom:0.5px solid #e0e0d8;}
.scard{background:#f3f4f2;border-radius:8px;padding:9px 12px;}
.sval{font-size:22px;font-weight:500;line-height:1;}
.slbl{font-size:11px;color:#888;margin-top:3px;}

/* ── Body layout ──────────────────────────── */
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

/* ── Modal overlay ────────────────────────── */
.modal-overlay{position:fixed;inset:0;background:rgba(0,0,0,.45);z-index:100;display:flex;align-items:center;justify-content:center;padding:16px;opacity:0;pointer-events:none;transition:opacity .18s;}
.modal-overlay.open{opacity:1;pointer-events:all;}
.modal{background:#fff;border-radius:12px;border:0.5px solid #d8d8d0;width:100%;max-width:680px;max-height:90vh;display:flex;flex-direction:column;overflow:hidden;}

/* ── Modal header ─────────────────────────── */
.modal-hdr{display:flex;align-items:flex-start;justify-content:space-between;padding:15px 20px;border-bottom:0.5px solid #e8e8e0;}
.modal-hdr-text h2{font-size:15px;font-weight:500;color:#1a1a1a;}
.modal-hdr-text p{font-size:12px;color:#888;margin-top:2px;}
.modal-close{width:28px;height:28px;border-radius:7px;border:0.5px solid #e0e0d8;background:transparent;cursor:pointer;display:flex;align-items:center;justify-content:center;color:#888;font-size:16px;line-height:1;flex-shrink:0;}
.modal-close:hover{background:#f3f4f2;}

/* ── Modal body: two columns ──────────────── */
.modal-body{display:grid;grid-template-columns:200px 1fr;flex:1;overflow:hidden;min-height:0;}

/* ── Bin list panel (left) ────────────────── */
.bin-panel{padding:12px 14px;border-right:0.5px solid #e8e8e0;overflow-y:auto;}
.panel-lbl{font-size:10px;font-weight:500;color:#aaa;text-transform:uppercase;letter-spacing:.06em;margin-bottom:8px;}
.bin-item{display:flex;align-items:center;justify-content:space-between;padding:6px 9px;border-radius:7px;border:0.5px solid #e8e8e0;background:#f8f8f4;margin-bottom:5px;}
.bin-item-left{display:flex;align-items:center;gap:7px;min-width:0;}
.bi-dot{width:7px;height:7px;border-radius:50%;flex-shrink:0;}
.bi-name{font-size:12px;color:#1a1a1a;white-space:nowrap;}
.bi-badge{font-size:10px;padding:1px 5px;border-radius:4px;font-weight:500;flex-shrink:0;}
.bi-badge-mwf{background:#e1f5ee;color:#0f6e56;}
.bi-badge-tth{background:#e6f1fb;color:#185fa5;}
.bi-badge-fs{background:#faeeda;color:#854f0b;}
.add-wrap{position:relative;flex-shrink:0;}
.add-btn{font-size:10px;padding:3px 7px;border-radius:5px;border:0.5px solid #d8d8d0;background:#fff;cursor:pointer;color:#555;white-space:nowrap;}
.add-btn:hover{border-color:#aaa;background:#f3f4f2;}
.dd-menu{position:absolute;right:0;top:calc(100% + 3px);background:#fff;border:0.5px solid #d8d8d0;border-radius:8px;min-width:140px;z-index:200;box-shadow:0 4px 14px rgba(0,0,0,.1);}
.dd-item{display:flex;align-items:center;gap:7px;width:100%;padding:7px 11px;font-size:12px;border:none;background:transparent;cursor:pointer;color:#1a1a1a;text-align:left;}
.dd-item:hover{background:#f3f4f2;}
.dd-item-badge{display:inline-block;padding:1px 5px;border-radius:4px;font-size:10px;font-weight:500;}

/* ── Schedule panel (right) ───────────────── */
.sched-panel{padding:12px 14px;overflow-y:auto;background:#f8f8f4;}

/* ── Day group card ───────────────────────── */
.day-card{background:#fff;border:0.5px solid #e8e8e0;border-radius:9px;margin-bottom:9px;overflow:hidden;}
.day-card-hdr{display:flex;align-items:center;justify-content:space-between;padding:8px 11px;border-bottom:0.5px solid #f0f0e8;}
.day-hdr-left{display:flex;align-items:center;gap:7px;}
.day-badge{font-size:11px;font-weight:500;padding:3px 8px;border-radius:5px;}
.day-sub{font-size:11px;color:#aaa;}
.day-count{font-size:11px;color:#aaa;}
.clr-btn{font-size:10px;padding:2px 7px;border:0.5px solid #e0e0d8;border-radius:5px;background:transparent;cursor:pointer;color:#aaa;}
.clr-btn:hover{border-color:#f09595;color:#c62828;}
.day-chips{padding:7px 9px;display:flex;flex-wrap:wrap;gap:5px;min-height:38px;align-items:center;}
.empty-hint{font-size:11px;color:#ccc;}
.chip{display:inline-flex;align-items:center;gap:4px;padding:3px 7px;border-radius:5px;border:0.5px solid #e8e8e0;background:#f8f8f4;font-size:11px;color:#333;}
.chip-dot{width:5px;height:5px;border-radius:50%;flex-shrink:0;}
.chip-x{border:none;background:transparent;color:#bbb;cursor:pointer;font-size:13px;line-height:1;padding:0;margin-left:1px;}
.chip-x:hover{color:#c62828;}

/* ── Modal footer ─────────────────────────── */
.modal-ftr{display:flex;align-items:center;justify-content:space-between;padding:11px 20px;border-top:0.5px solid #e8e8e0;background:#fff;}
.ftr-note{font-size:12px;color:#aaa;}
.ftr-btns{display:flex;gap:7px;}
.btn-sec{padding:6px 13px;border-radius:7px;border:0.5px solid #d8d8d0;background:#fff;cursor:pointer;font-size:12px;color:#555;}
.btn-sec:hover{background:#f3f4f2;}
.btn-pri{padding:6px 13px;border-radius:7px;border:none;background:#1d9e75;cursor:pointer;font-size:12px;font-weight:500;color:#fff;}
.btn-pri:hover{background:#0f6e56;}
.btn-pri:disabled{background:#b0d8c8;cursor:default;}
</style>
</head>
<body>

<!-- ══════════════════════════════════════════
     HEADER
══════════════════════════════════════════════ -->
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

  <!-- Schedule button -->
  <button class="btn-sched" onclick="openSchedModal()">
    <svg width="13" height="13" viewBox="0 0 13 13" fill="none">
      <rect x="1" y="2" width="11" height="10" rx="1.5" stroke="currentColor" stroke-width="1.1" fill="none"/>
      <line x1="4" y1="1" x2="4" y2="4" stroke="currentColor" stroke-width="1.1" stroke-linecap="round"/>
      <line x1="9" y1="1" x2="9" y2="4" stroke="currentColor" stroke-width="1.1" stroke-linecap="round"/>
      <line x1="1" y1="5.5" x2="12" y2="5.5" stroke="currentColor" stroke-width="1.1"/>
      <line x1="4" y1="8" x2="9" y2="8" stroke="currentColor" stroke-width="1" stroke-linecap="round"/>
    </svg>
    Schedule
  </button>
</div>

<!-- ══════════════════════════════════════════
     STATS
══════════════════════════════════════════════ -->
<div class="stats-row">
  <div class="scard"><div class="sval" id="st-total">16</div><div class="slbl">Total bins</div></div>
  <div class="scard"><div class="sval" id="st-ok"   style="color:#2e7d32">--</div><div class="slbl">Online</div></div>
  <div class="scard"><div class="sval" id="st-full" style="color:#c62828">--</div><div class="slbl">Full &ge;90%</div></div>
  <div class="scard"><div class="sval" id="st-warn" style="color:#e65100">--</div><div class="slbl">Warning &ge;70%</div></div>
  <div class="scard"><div class="sval" id="st-avg">--%</div><div class="slbl">Avg fill</div></div>
</div>

<!-- ══════════════════════════════════════════
     MAIN BODY
══════════════════════════════════════════════ -->
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

<!-- ══════════════════════════════════════════
     SCHEDULING MODAL
══════════════════════════════════════════════ -->
<div class="modal-overlay" id="sched-overlay" onclick="overlayClick(event)">
  <div class="modal">

    <div class="modal-hdr">
      <div class="modal-hdr-text">
        <h2>Bin Collection Schedule</h2>
        <p>Assign bins to collection days — MWF, TTH, or FS</p>
      </div>
      <button class="modal-close" onclick="closeSchedModal()">&#x2715;</button>
    </div>

    <div class="modal-body">
      <!-- Left: bin list -->
      <div class="bin-panel">
        <div class="panel-lbl">Available bins</div>
        <div id="modal-bin-list"></div>
      </div>

      <!-- Right: schedule -->
      <div class="sched-panel">
        <div id="modal-sched-groups"></div>
      </div>
    </div>

    <div class="modal-ftr">
      <span class="ftr-note" id="modal-ftr-note">0 bins assigned</span>
      <div class="ftr-btns">
        <button class="btn-sec" onclick="clearAllSched()">Clear all</button>
        <button class="btn-pri" id="save-btn" onclick="saveSchedule()">Save schedule</button>
      </div>
    </div>

  </div>
</div>

<!-- ══════════════════════════════════════════
     JAVASCRIPT
══════════════════════════════════════════════ -->
<script>
// ── Dashboard constants ──────────────────────
const REFRESH_MS = 2000;

// ── Bin list (matches initBins in main.cpp) ──
const ALL_BINS = [
  {id:0,name:"Bin A1",zone:"a"},{id:1,name:"Bin A2",zone:"a"},
  {id:2,name:"Bin A3",zone:"a"},{id:3,name:"Bin A4",zone:"a"},
  {id:4,name:"Bin A5",zone:"a"},{id:5,name:"Bin A6",zone:"a"},
  {id:6,name:"Bin A7",zone:"a"},{id:7,name:"Bin A8",zone:"a"},
  {id:8,name:"Bin B1",zone:"b"},{id:9,name:"Bin B2",zone:"b"},
  {id:10,name:"Bin B3",zone:"b"},{id:11,name:"Bin B4",zone:"b"},
  {id:12,name:"Bin B5",zone:"b"},{id:13,name:"Bin B6",zone:"b"},
  {id:14,name:"Bin B7",zone:"b"},{id:15,name:"Bin B8",zone:"b"}
];

// ── Day definitions ──────────────────────────
const DAYS = [
  {key:"mwf",label:"Mon / Wed / Fri",badge:"MWF",badgeCls:"bi-badge-mwf",dot:"#1d9e75"},
  {key:"tth",label:"Tue / Thu",      badge:"TTH",badgeCls:"bi-badge-tth",dot:"#378add"},
  {key:"fs", label:"Fri / Sat",      badge:"FS", badgeCls:"bi-badge-fs", dot:"#ef9f27"}
];

// ── Schedule state: {mwf:[], tth:[], fs:[]} ──
let schedule = {mwf:[],tth:[],fs:[]};
let activeDD  = null; // currently open dropdown bin id

// ════════════════════════════════════════════
// Dashboard rendering
// ════════════════════════════════════════════
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
    const cx = b.x * 7.6, cy = b.y * 4.0;
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

// ════════════════════════════════════════════
// Schedule modal logic
// ════════════════════════════════════════════

function dotColor(zone) { return zone === 'a' ? '#1d9e75' : '#378add'; }

function getAssignedDay(binId) {
  for (const d of DAYS) { if (schedule[d.key].includes(binId)) return d.key; }
  return null;
}

function totalAssigned() {
  const s = new Set();
  DAYS.forEach(d => schedule[d.key].forEach(id => s.add(id)));
  return s.size;
}

// ── Open modal & load current schedule from ESP ──
async function openSchedModal() {
  // Try to load existing schedule from the server
  try {
    const res  = await fetch('/api/schedule');
    if (res.ok) {
      const data = await res.json();
      schedule.mwf = data.mwf || [];
      schedule.tth = data.tth || [];
      schedule.fs  = data.fs  || [];
    }
  } catch (_) { /* server may not have one yet, use empty */ }

  renderModal();
  document.getElementById('sched-overlay').classList.add('open');
}

function closeSchedModal() {
  document.getElementById('sched-overlay').classList.remove('open');
  closeDD();
}

// Close modal on backdrop click
function overlayClick(e) {
  if (e.target === document.getElementById('sched-overlay')) closeSchedModal();
}

// ── Assign bin to a day, removing from any other ──
function assignBin(binId, dayKey) {
  DAYS.forEach(d => { schedule[d.key] = schedule[d.key].filter(id => id !== binId); });
  if (!schedule[dayKey].includes(binId)) schedule[dayKey].push(binId);
  closeDD();
  renderModal();
}

function removeChip(binId, dayKey) {
  schedule[dayKey] = schedule[dayKey].filter(id => id !== binId);
  renderModal();
}

function clearDay(dayKey) {
  schedule[dayKey] = [];
  renderModal();
}

function clearAllSched() {
  schedule = {mwf:[],tth:[],fs:[]};
  renderModal();
}

// ── Dropdown helpers ─────────────────────────────
function toggleDD(binId, event) {
  event.stopPropagation();
  if (activeDD === binId) { closeDD(); return; }
  closeDD();
  activeDD = binId;
  const el = document.getElementById('dd-' + binId);
  if (el) el.style.display = 'block';
}

function closeDD() {
  if (activeDD !== null) {
    const el = document.getElementById('dd-' + activeDD);
    if (el) el.style.display = 'none';
  }
  activeDD = null;
}

document.addEventListener('click', closeDD);

// ── Render entire modal content ──────────────────
function renderModal() {
  renderModalBinList();
  renderModalSchedGroups();
  const n = totalAssigned();
  document.getElementById('modal-ftr-note').textContent =
    n + ' bin' + (n !== 1 ? 's' : '') + ' assigned';
}

function renderModalBinList() {
  const el = document.getElementById('modal-bin-list');
  el.innerHTML = '';
  ALL_BINS.forEach(bin => {
    const assigned  = getAssignedDay(bin.id);
    const dayInfo   = assigned ? DAYS.find(d => d.key === assigned) : null;
    const btnLabel  = assigned ? 'Move' : '+ Add';

    const row = document.createElement('div');
    row.className = 'bin-item';
    row.innerHTML = `
      <div class="bin-item-left">
        <span class="bi-dot" style="background:${dotColor(bin.zone)}"></span>
        <span class="bi-name">${bin.name}</span>
        ${assigned ? `<span class="bi-badge ${dayInfo.badgeCls}">${dayInfo.badge}</span>` : ''}
      </div>
      <div class="add-wrap" id="wrap-${bin.id}">
        <button class="add-btn" onclick="toggleDD(${bin.id},event)">${btnLabel}</button>
        <div class="dd-menu" id="dd-${bin.id}" style="display:none" onclick="event.stopPropagation()">
          ${DAYS.map(d => `
            <button class="dd-item" onclick="assignBin(${bin.id},'${d.key}')">
              <span class="dd-item-badge ${d.badgeCls}">${d.badge}</span>
              ${d.label}
            </button>`).join('')}
        </div>
      </div>`;
    el.appendChild(row);
  });
}

function renderModalSchedGroups() {
  const el = document.getElementById('modal-sched-groups');
  el.innerHTML = '';

  DAYS.forEach(day => {
    const binIds = schedule[day.key];
    const card   = document.createElement('div');
    card.className = 'day-card';

    const chipsHtml = binIds.length
      ? binIds.map(id => {
          const bin = ALL_BINS.find(b => b.id === id);
          if (!bin) return '';
          return `<span class="chip">
            <span class="chip-dot" style="background:${dotColor(bin.zone)}"></span>
            ${bin.name}
            <button class="chip-x" onclick="removeChip(${id},'${day.key}')" title="Remove">&#x2715;</button>
          </span>`;
        }).join('')
      : '<span class="empty-hint">No bins assigned</span>';

    card.innerHTML = `
      <div class="day-card-hdr">
        <div class="day-hdr-left">
          <span class="day-badge ${day.badgeCls}">${day.badge}</span>
          <span class="day-sub">${day.label}</span>
        </div>
        <div style="display:flex;align-items:center;gap:7px;">
          <span class="day-count">${binIds.length} bin${binIds.length !== 1 ? 's' : ''}</span>
          ${binIds.length ? `<button class="clr-btn" onclick="clearDay('${day.key}')">Clear</button>` : ''}
        </div>
      </div>
      <div class="day-chips">${chipsHtml}</div>`;
    el.appendChild(card);
  });
}

// ── POST schedule to ESP32 ───────────────────────
async function saveSchedule() {
  const btn = document.getElementById('save-btn');
  btn.disabled = true;
  btn.textContent = 'Saving...';

  try {
    const res = await fetch('/api/schedule', {
      method:  'POST',
      headers: {'Content-Type':'application/json'},
      body:    JSON.stringify(schedule)
    });
    if (res.ok) {
      btn.textContent = 'Saved!';
      setTimeout(() => {
        btn.disabled    = false;
        btn.textContent = 'Save schedule';
        closeSchedModal();
      }, 900);
    } else {
      throw new Error('Server error');
    }
  } catch (_) {
    btn.disabled    = false;
    btn.textContent = 'Save schedule';
    alert('Could not save schedule. Check connection.');
  }
}
</script>
</body>
</html>
)rawhtml";
}
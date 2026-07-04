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

.hdr{display:flex;align-items:center;gap:10px;padding:11px 18px;background:#fff;border-bottom:0.5px solid #e0e0d8;}
.hdr-title{font-size:15px;font-weight:500;}
.hdr-sub{font-size:12px;color:#888;margin-left:4px;}
.badge{font-size:11px;padding:2px 8px;border-radius:20px;font-weight:500;}
.badge-ok{background:#e8f5e9;color:#2e7d32;}
.badge-err{background:#ffebee;color:#c62828;animation:pulse 1s infinite;}
@keyframes pulse{0%,100%{opacity:1}50%{opacity:.4}}

.btn-sched{display:flex;align-items:center;gap:5px;padding:5px 11px;border-radius:7px;border:0.5px solid #d0d0c8;background:#fff;font-size:12px;font-weight:500;color:#444;cursor:pointer;transition:background .12s;}
.btn-sched:hover{background:#f3f4f2;}

.stats-row{display:grid;grid-template-columns:repeat(5,minmax(0,1fr));gap:10px;padding:10px 18px;background:#fff;border-bottom:0.5px solid #e0e0d8;}
.scard{background:#f3f4f2;border-radius:8px;padding:9px 12px;}
.sval{font-size:22px;font-weight:500;line-height:1;}
.slbl{font-size:11px;color:#888;margin-top:3px;}

.body{display:flex;flex:1;overflow:hidden;}
.map-wrap{flex:1;padding:12px 16px;overflow:hidden;display:flex;flex-direction:column;gap:10px;}
.map-lbl{font-size:11px;color:#888;font-weight:500;letter-spacing:.04em;text-transform:uppercase;}
.zone-block{flex:1;display:flex;flex-direction:column;min-height:0;}
.zone-hdr{display:flex;align-items:center;gap:8px;margin-bottom:5px;}
.zone-badge{font-size:10px;font-weight:600;padding:2px 8px;border-radius:4px;letter-spacing:.05em;}
.zone-badge-a{background:#e1f5ee;color:#0f6e56;}
.zone-badge-b{background:#e6f1fb;color:#185fa5;}
.zone-conn{font-size:10px;padding:2px 7px;border-radius:20px;font-weight:500;}
.zone-conn-ok{background:#e8f5e9;color:#2e7d32;}
.zone-conn-err{background:#ffebee;color:#c62828;}
.map-area{background:#fff;border:0.5px solid #e0e0d8;border-radius:10px;overflow:hidden;flex:1;}
.map-svg{width:100%;height:100%;}

.sidebar{width:252px;border-left:0.5px solid #e0e0d8;overflow-y:auto;background:#fff;}
.side-hdr{font-size:11px;color:#888;padding:9px 14px;border-bottom:0.5px solid #e0e0d8;font-weight:500;letter-spacing:.04em;text-transform:uppercase;position:sticky;top:0;background:#fff;z-index:2;}
.side-zone-lbl{font-size:10px;font-weight:600;padding:6px 14px 3px;letter-spacing:.06em;text-transform:uppercase;color:#aaa;background:#f8f8f4;border-bottom:0.5px solid #f0f0e8;}
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

.pick-btn{display:inline-flex;align-items:center;gap:4px;margin-top:4px;padding:3px 8px;border-radius:5px;border:none;background:#c62828;color:#fff;font-size:10px;font-weight:500;cursor:pointer;transition:background .12s,opacity .15s;animation:pickIn .2s cubic-bezier(.22,1,.36,1);}
.pick-btn:hover{background:#a31515;}
.pick-btn:active{opacity:.75;}
.pick-btn:disabled{background:#e57373;cursor:default;}
@keyframes pickIn{from{opacity:0;transform:translateY(4px);}to{opacity:1;transform:none;}}

.modal-overlay{position:fixed;inset:0;background:rgba(0,0,0,.45);z-index:100;display:flex;align-items:center;justify-content:center;padding:16px;opacity:0;pointer-events:none;transition:opacity .18s;}
.modal-overlay.open{opacity:1;pointer-events:all;}
.modal{background:#fff;border-radius:12px;border:0.5px solid #d8d8d0;width:100%;max-width:720px;max-height:90vh;display:flex;flex-direction:column;overflow:hidden;}

.modal-hdr{display:flex;align-items:flex-start;justify-content:space-between;padding:15px 20px;border-bottom:0.5px solid #e8e8e0;}
.modal-hdr-text h2{font-size:15px;font-weight:500;color:#1a1a1a;}
.modal-hdr-text p{font-size:12px;color:#888;margin-top:2px;}
.modal-close{width:28px;height:28px;border-radius:7px;border:0.5px solid #e0e0d8;background:transparent;cursor:pointer;display:flex;align-items:center;justify-content:center;color:#888;font-size:16px;line-height:1;flex-shrink:0;}
.modal-close:hover{background:#f3f4f2;}

.modal-tabs{display:flex;border-bottom:0.5px solid #e8e8e0;padding:0 20px;background:#fff;}
.tab-btn{padding:9px 14px;font-size:12px;font-weight:500;color:#888;border:none;background:transparent;cursor:pointer;border-bottom:2px solid transparent;margin-bottom:-0.5px;transition:color .12s,border-color .12s;}
.tab-btn.active{color:#1a1a1a;border-bottom-color:#1d9e75;}
.tab-btn:hover:not(.active){color:#555;}
.tab-content{display:none;flex:1;overflow:hidden;min-height:0;}
.tab-content.active{display:flex;}

.modal-body{display:grid;grid-template-columns:200px 1fr;flex:1;overflow:hidden;min-height:0;width:100%;}

.bin-panel{padding:12px 14px;border-right:0.5px solid #e8e8e0;overflow-y:auto;}
.panel-lbl{font-size:10px;font-weight:500;color:#aaa;text-transform:uppercase;letter-spacing:.06em;margin-bottom:6px;}
.panel-zone-lbl{font-size:10px;font-weight:600;color:#aaa;text-transform:uppercase;letter-spacing:.06em;margin:10px 0 5px;padding-bottom:3px;border-bottom:0.5px solid #f0f0e8;}
.panel-zone-lbl:first-of-type{margin-top:0;}
.bin-item{display:flex;align-items:center;justify-content:space-between;padding:6px 9px;border-radius:7px;border:0.5px solid #e8e8e0;background:#f8f8f4;margin-bottom:5px;}
.bin-item-left{display:flex;align-items:center;gap:7px;min-width:0;}
.bi-dot{width:7px;height:7px;border-radius:50%;flex-shrink:0;}
.bi-name{font-size:12px;color:#1a1a1a;white-space:nowrap;}
.bi-badge{font-size:10px;padding:1px 5px;border-radius:4px;font-weight:500;flex-shrink:0;}
.bi-badge-mwf{background:#e1f5ee;color:#0f6e56;}
.bi-badge-tth{background:#e6f1fb;color:#185fa5;}
.bi-badge-fs{background:#faeeda;color:#854f0b;}
.bi-badge-comm{background:#fce4ec;color:#b71c1c;}
.add-wrap{position:relative;flex-shrink:0;}
.add-btn{font-size:10px;padding:3px 7px;border-radius:5px;border:0.5px solid #d8d8d0;background:#fff;cursor:pointer;color:#555;white-space:nowrap;}
.add-btn:hover{border-color:#aaa;background:#f3f4f2;}
.dd-menu{position:absolute;right:0;top:calc(100% + 3px);background:#fff;border:0.5px solid #d8d8d0;border-radius:8px;min-width:140px;z-index:200;box-shadow:0 4px 14px rgba(0,0,0,.1);}
.dd-item{display:flex;align-items:center;gap:7px;width:100%;padding:7px 11px;font-size:12px;border:none;background:transparent;cursor:pointer;color:#1a1a1a;text-align:left;}
.dd-item:hover{background:#f3f4f2;}
.dd-item-badge{display:inline-block;padding:1px 5px;border-radius:4px;font-size:10px;font-weight:500;}

.sched-panel{padding:12px 14px;overflow-y:auto;background:#f8f8f4;}
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

.comm-tab-body{flex:1;overflow-y:auto;padding:14px 16px;background:#f8f8f4;width:100%;}
.comm-intro{background:#fff3e0;border:0.5px solid #ffe082;border-radius:9px;padding:10px 13px;margin-bottom:12px;display:flex;gap:9px;align-items:flex-start;}
.comm-intro-icon{font-size:16px;flex-shrink:0;margin-top:1px;}
.comm-intro-text{font-size:12px;color:#6d4c00;line-height:1.55;}
.comm-intro-text strong{font-weight:600;}
.comm-zone-card{background:#fff;border:0.5px solid #e8e8e0;border-radius:9px;margin-bottom:9px;overflow:hidden;}
.comm-zone-hdr{display:flex;align-items:center;justify-content:space-between;padding:8px 12px;border-bottom:0.5px solid #f0f0e8;}
.comm-zone-title{font-size:12px;font-weight:500;color:#1a1a1a;display:flex;align-items:center;gap:7px;}
.comm-zone-dot{width:8px;height:8px;border-radius:50%;}
.comm-zone-sub{font-size:11px;color:#aaa;}
.comm-grid{display:grid;grid-template-columns:repeat(4,1fr);gap:7px;padding:10px 12px;}
.comm-bin-card{border:0.5px solid #e8e8e0;border-radius:7px;padding:8px 9px;background:#f8f8f4;cursor:pointer;transition:border-color .12s,background .12s;position:relative;user-select:none;}
.comm-bin-card:hover{background:#f0f0e8;border-color:#ccc;}
.comm-bin-card.selected{border-color:#b71c1c;background:#fff5f5;}
.comm-bin-card.selected .comm-check{opacity:1;}
.comm-check{position:absolute;top:6px;right:7px;width:14px;height:14px;border-radius:50%;background:#b71c1c;display:flex;align-items:center;justify-content:center;opacity:0;transition:opacity .12s;}
.comm-check svg{display:block;}
.comm-bin-name{font-size:11px;font-weight:500;color:#1a1a1a;}
.comm-bin-fill{font-size:10px;color:#888;margin-top:2px;}
.comm-count-note{font-size:11px;color:#aaa;padding:0 12px 10px;text-align:right;}

.modal-ftr{display:flex;align-items:center;justify-content:space-between;padding:11px 20px;border-top:0.5px solid #e8e8e0;background:#fff;}
.ftr-note{font-size:12px;color:#aaa;}
.ftr-btns{display:flex;gap:7px;}
.btn-sec{padding:6px 13px;border-radius:7px;border:0.5px solid #d8d8d0;background:#fff;cursor:pointer;font-size:12px;color:#555;}
.btn-sec:hover{background:#f3f4f2;}
.btn-pri{padding:6px 13px;border-radius:7px;border:none;background:#1d9e75;cursor:pointer;font-size:12px;font-weight:500;color:#fff;}
.btn-pri:hover{background:#0f6e56;}
.btn-pri:disabled{background:#b0d8c8;cursor:default;}

.alert-banner{position:fixed;top:0;left:50%;transform:translateX(-50%);z-index:300;display:flex;flex-direction:column;gap:6px;padding-top:10px;pointer-events:none;width:100%;max-width:520px;padding-left:16px;padding-right:16px;}
.alert-item{display:flex;align-items:flex-start;gap:10px;background:#fff;border:1px solid #f44336;border-left:4px solid #c62828;border-radius:9px;padding:10px 12px;box-shadow:0 4px 16px rgba(198,40,40,.13);pointer-events:all;animation:alertIn .22s cubic-bezier(.22,1,.36,1);}
@keyframes alertIn{from{opacity:0;transform:translateY(-12px);}to{opacity:1;transform:none;}}
.alert-icon{font-size:16px;flex-shrink:0;margin-top:1px;}
.alert-body{flex:1;min-width:0;}
.alert-title{font-size:12px;font-weight:600;color:#c62828;}
.alert-msg{font-size:11px;color:#555;margin-top:2px;line-height:1.45;}
.alert-close{border:none;background:transparent;color:#aaa;cursor:pointer;font-size:15px;line-height:1;padding:0;flex-shrink:0;align-self:flex-start;}
.alert-close:hover{color:#c62828;}
</style>
</head>
<body>

<div class="alert-banner" id="alert-banner"></div>

<!-- HEADER -->
<div class="hdr">
  <svg width="18" height="18" viewBox="0 0 18 18" fill="none">
    <rect x="3" y="5" width="12" height="11" rx="1.5" stroke="currentColor" stroke-width="1.2" fill="none"/>
    <path d="M6 5V4a1 1 0 0 1 1-1h4a1 1 0 0 1 1 1v1" stroke="currentColor" stroke-width="1.2" fill="none"/>
    <line x1="2" y1="5" x2="16" y2="5" stroke="currentColor" stroke-width="1.2" stroke-linecap="round"/>
  </svg>
  <span class="hdr-title">TrashBin Monitor</span>
  <span class="hdr-sub">Zone A &amp; B &middot; 8 bins</span>
  <span class="badge badge-ok" style="margin-left:auto" id="conn-badge">Live</span>
  <span style="font-size:11px;color:#aaa;margin-left:8px" id="clk"></span>
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

<!-- STATS -->
<div class="stats-row">
  <div class="scard"><div class="sval" id="st-total">8</div><div class="slbl">Total bins</div></div>
  <div class="scard"><div class="sval" id="st-ok"   style="color:#2e7d32">--</div><div class="slbl">Online</div></div>
  <div class="scard"><div class="sval" id="st-full" style="color:#c62828">--</div><div class="slbl">Full &ge;90%</div></div>
  <div class="scard"><div class="sval" id="st-warn" style="color:#e65100">--</div><div class="slbl">Warning &ge;70%</div></div>
  <div class="scard"><div class="sval" id="st-avg">--%</div><div class="slbl">Avg fill</div></div>
</div>

<!-- MAIN BODY -->
<div class="body">
  <div class="map-wrap">

    <!-- ZONE A -->
    <div class="zone-block">
      <div class="zone-hdr">
        <span class="zone-badge zone-badge-a">ZONE A</span>
        <span class="map-lbl">Floor plan</span>
        <span class="zone-conn zone-conn-ok" id="za-badge">&#x25CF; Online</span>
      </div>
      <div class="map-area" style="position:relative;">
        <svg class="map-svg" viewBox="0 0 760 140" xmlns="http://www.w3.org/2000/svg">
          <rect width="760" height="140" fill="#f8f8f4"/>
          <text x="10" y="13" font-family="system-ui,sans-serif" font-size="9" fill="#bbb" letter-spacing="1">ZONE A</text>
          <g id="mbins-a"></g>
        </svg>
        <div class="tip" id="tip-a"></div>
      </div>
    </div>

    <!-- ZONE B -->
    <div class="zone-block">
      <div class="zone-hdr">
        <span class="zone-badge zone-badge-b">ZONE B</span>
        <span class="map-lbl">Floor plan</span>
        <span class="zone-conn zone-conn-ok" id="zb-badge">&#x25CF; Online</span>
      </div>
      <div class="map-area" style="position:relative;">
        <svg class="map-svg" viewBox="0 0 760 140" xmlns="http://www.w3.org/2000/svg">
          <rect width="760" height="140" fill="#f8f8f4"/>
          <text x="10" y="13" font-family="system-ui,sans-serif" font-size="9" fill="#bbb" letter-spacing="1">ZONE B</text>
          <g id="mbins-b"></g>
        </svg>
        <div class="tip" id="tip-b"></div>
      </div>
    </div>

  </div>

  <!-- SIDEBAR -->
  <div class="sidebar">
    <div class="side-hdr">Bin status list</div>
    <div class="side-zone-lbl">Zone A</div>
    <div id="blist-a"></div>
    <div class="side-zone-lbl">Zone B</div>
    <div id="blist-b"></div>
  </div>
</div>

<!-- SCHEDULING MODAL -->
<div class="modal-overlay" id="sched-overlay" onclick="overlayClick(event)">
  <div class="modal">
    <div class="modal-hdr">
      <div class="modal-hdr-text">
        <h2>Bin Collection Schedule</h2>
        <p>Assign bins to collection days and mark commercial bins for priority alerts</p>
      </div>
      <button class="modal-close" onclick="closeSchedModal()">&#x2715;</button>
    </div>

    <div class="modal-tabs">
      <button class="tab-btn active" id="tab-sched-btn" onclick="switchTab('sched')">Collection Schedule</button>
      <button class="tab-btn" id="tab-comm-btn" onclick="switchTab('comm')">
        Commercial Bins
        <span id="comm-count-badge" style="display:none;margin-left:5px;background:#fce4ec;color:#b71c1c;font-size:10px;padding:1px 5px;border-radius:4px;font-weight:600;"></span>
      </button>
    </div>

    <div class="tab-content active" id="tab-sched">
      <div class="modal-body">
        <div class="bin-panel">
          <div id="modal-bin-list"></div>
        </div>
        <div class="sched-panel">
          <div id="modal-sched-groups"></div>
        </div>
      </div>
    </div>

    <div class="tab-content" id="tab-comm">
      <div class="comm-tab-body">
        <div class="comm-intro">
          <div class="comm-intro-icon">&#9888;&#65039;</div>
          <div class="comm-intro-text">
            <strong>Commercial bins</strong> are high-priority bins from businesses or high-traffic areas.
            When a commercial bin is <strong>full (&ge;90%)</strong> and <strong>not yet scheduled</strong>,
            an immediate alert will appear on the dashboard.
          </div>
        </div>
        <div id="comm-zone-list"></div>
      </div>
    </div>

    <div class="modal-ftr">
      <span class="ftr-note" id="modal-ftr-note">0 bins assigned</span>
      <div class="ftr-btns">
        <button class="btn-sec" id="ftr-clear-btn" onclick="clearAllSched()">Clear all</button>
        <button class="btn-pri" id="save-btn" onclick="saveSchedule()">Save schedule</button>
      </div>
    </div>
  </div>
</div>

<script>
const REFRESH_MS   = 2000;
const ESP32_A_IP   = "10.37.96.254";

// All 8 bins — A = ids 0-3, B = ids 4-7
const ALL_BINS = [
  {id:0, name:"Bin A1", zone:"a"},
  {id:1, name:"Bin A2", zone:"a"},
  {id:2, name:"Bin A3", zone:"a"},
  {id:3, name:"Bin A4", zone:"a"},
  {id:4, name:"Bin B1", zone:"b"},
  {id:5, name:"Bin B2", zone:"b"},
  {id:6, name:"Bin B3", zone:"b"},
  {id:7, name:"Bin B4", zone:"b"}
];

const DAYS = [
  {key:"mwf", label:"Mon / Wed / Fri", badge:"MWF", badgeCls:"bi-badge-mwf", dot:"#1d9e75"},
  {key:"tth", label:"Tue / Thu",       badge:"TTH", badgeCls:"bi-badge-tth", dot:"#378add"},
  {key:"fs",  label:"Fri / Sat",       badge:"FS",  badgeCls:"bi-badge-fs",  dot:"#ef9f27"}
];

// Map X positions for each bin within a zone SVG (viewBox 760x140)
const ZONE_MAP_POS = [
  {cx:152, cy:70},
  {cx:304, cy:70},
  {cx:456, cy:70},
  {cx:608, cy:70}
];

let schedule        = {mwf:[], tth:[], fs:[]};
let commercials     = [];
let activeDD        = null;
let activeTab       = 'sched';
let lastBinsData    = [];
let dismissedAlerts = new Set();

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

// ── Render a zone's SVG map (bins is the subset for that zone, localIdx 0-3) ──
function renderZoneMap(zoneBins, svgGroupId, tipId, mapAreaSelector) {
  const g  = document.getElementById(svgGroupId);
  if (!g) return;
  g.innerHTML = '';
  const NS = 'http://www.w3.org/2000/svg';

  zoneBins.forEach((b, localIdx) => {
    const pos    = ZONE_MAP_POS[localIdx] || {cx:80+localIdx*160, cy:70};
    const cx     = pos.cx, cy = pos.cy;
    const col    = statusColor(b.fill);
    const r = 28, arcR = 20;
    const circ   = 2 * Math.PI * arcR;
    const dash   = (b.fill / 100) * circ;
    const isComm = commercials.includes(b.id);

    const bg = document.createElementNS(NS, 'circle');
    bg.setAttribute('cx', cx); bg.setAttribute('cy', cy); bg.setAttribute('r', r);
    bg.setAttribute('fill', '#fff');
    bg.setAttribute('stroke', isComm ? '#e57373' : '#ddd');
    bg.setAttribute('stroke-width', isComm ? '1.5' : '0.5');
    g.appendChild(bg);

    const arc = document.createElementNS(NS, 'circle');
    arc.setAttribute('cx', cx); arc.setAttribute('cy', cy); arc.setAttribute('r', arcR);
    arc.setAttribute('fill', 'none'); arc.setAttribute('stroke', col); arc.setAttribute('stroke-width', '6');
    arc.setAttribute('stroke-dasharray', `${dash} ${circ}`);
    arc.setAttribute('stroke-dashoffset', `${circ * 0.25}`);
    arc.setAttribute('transform', `rotate(-90 ${cx} ${cy})`);
    g.appendChild(arc);

    const pct = document.createElementNS(NS, 'text');
    pct.setAttribute('x', cx); pct.setAttribute('y', cy + 4);
    pct.setAttribute('text-anchor', 'middle');
    pct.setAttribute('font-family', 'system-ui,sans-serif');
    pct.setAttribute('font-size', '11'); pct.setAttribute('font-weight', '500');
    pct.setAttribute('fill', col);
    pct.textContent = b.fill + '%';
    g.appendChild(pct);

    const lbl = document.createElementNS(NS, 'text');
    lbl.setAttribute('x', cx); lbl.setAttribute('y', cy + r + 14);
    lbl.setAttribute('text-anchor', 'middle');
    lbl.setAttribute('font-family', 'system-ui,sans-serif');
    lbl.setAttribute('font-size', '11');
    lbl.setAttribute('fill', '#888');
    lbl.textContent = b.name + (isComm ? ' \u2605' : '');
    g.appendChild(lbl);

    if (!b.ok) {
      const offLine = document.createElementNS(NS, 'text');
      offLine.setAttribute('x', cx); offLine.setAttribute('y', cy + r + 26);
      offLine.setAttribute('text-anchor', 'middle');
      offLine.setAttribute('font-family', 'system-ui,sans-serif');
      offLine.setAttribute('font-size', '9');
      offLine.setAttribute('fill', '#bbb');
      offLine.textContent = 'offline';
      g.appendChild(offLine);
    }

    const hit = document.createElementNS(NS, 'circle');
    hit.setAttribute('cx', cx); hit.setAttribute('cy', cy);
    hit.setAttribute('r', r + 10); hit.setAttribute('fill', 'transparent');
    hit.style.cursor = 'pointer';
    hit.addEventListener('mouseenter', e => showTip(e, b, tipId, mapAreaSelector));
    hit.addEventListener('mouseleave', () => document.getElementById(tipId).classList.remove('show'));
    hit.addEventListener('click', () => hlRow(b.id));
    g.appendChild(hit);
  });
}

function showTip(e, b, tipId, mapAreaSelector) {
  const tip    = document.getElementById(tipId);
  const rect   = document.querySelector(mapAreaSelector).getBoundingClientRect();
  const col    = statusColor(b.fill);
  const isComm = commercials.includes(b.id);
  tip.innerHTML = `<b>${b.name}</b>${isComm ? ' <span style="color:#b71c1c;font-size:10px;">\u2605 Commercial</span>' : ''}<br>Fill: <span style="color:${col};font-weight:500">${b.fill}%</span> \u2014 ${statusLabel(b.fill)}<br>Dist: ${b.dist} mm<br>Sensor: ${b.ok ? '<span style="color:#2e7d32">Online</span>' : '<span style="color:#bbb">Offline</span>'}`;
  tip.style.left = (e.clientX - rect.left + 14) + 'px';
  tip.style.top  = (e.clientY - rect.top  - 10) + 'px';
  tip.classList.add('show');
}

function renderMap(bins) {
  const zoneA = bins.filter(b => b.id <= 3);
  const zoneB = bins.filter(b => b.id >= 4);
  renderZoneMap(zoneA, 'mbins-a', 'tip-a', '#tip-a');
  renderZoneMap(zoneB, 'mbins-b', 'tip-b', '#tip-b');

  // Update zone connection badges
  const aOnline = zoneA.some(b => b.ok);
  const bOnline = zoneB.some(b => b.ok);
  const zaEl = document.getElementById('za-badge');
  const zbEl = document.getElementById('zb-badge');
  zaEl.className = 'zone-conn ' + (aOnline ? 'zone-conn-ok' : 'zone-conn-err');
  zaEl.textContent = aOnline ? '\u25CF Online' : '\u25CF Offline';
  zbEl.className = 'zone-conn ' + (bOnline ? 'zone-conn-ok' : 'zone-conn-err');
  zbEl.textContent = bOnline ? '\u25CF Online' : '\u25CF Offline';
}

function renderListZone(bins, containerId, pickIds) {
  const el = document.getElementById(containerId);
  if (!el) return;
  el.innerHTML = '';
  bins.forEach(b => {
    const col      = statusColor(b.fill);
    const isComm   = commercials.includes(b.id);
    const showPick = pickIds.has(b.id) && b.ok && (isComm || b.full);

    const row = document.createElement('div');
    row.className = 'brow'; row.id = 'row-' + b.id;
    row.innerHTML = `
      <div class="bdot" style="background:${b.ok ? col : '#ccc'}"></div>
      <div style="flex:1;min-width:0">
        <div class="bname">${b.name}${isComm ? ' <span style="font-size:9px;background:#fce4ec;color:#b71c1c;padding:1px 4px;border-radius:3px;font-weight:500;">COMM</span>' : ''}</div>
        <div class="bstatus">${b.ok ? statusLabel(b.fill) : 'Offline'}</div>
        ${showPick ? `<button class="pick-btn" id="pick-btn-${b.id}" onclick="triggerPick(event,${b.id},'${b.name}')">&#x1F9A8; Pick Up</button>` : ''}
      </div>
      <div>
        <div class="bpct" style="color:${col}">${b.ok ? b.fill + '%' : '--'}</div>
        <div class="bbar-bg"><div class="bbar-fill" style="width:${b.fill}%;background:${col}"></div></div>
      </div>`;
    row.addEventListener('click', () => hlRow(b.id));
    el.appendChild(row);
  });
}
function renderList(bins) {
  const PICK_IDS = new Set([0,1,2,3,4,5,6,7]);
  renderListZone(bins.filter(b => b.id <= 3), 'blist-a', PICK_IDS);
  renderListZone(bins.filter(b => b.id >= 4), 'blist-b', PICK_IDS);
}

function hlRow(id) {
  document.querySelectorAll('.brow').forEach(r => r.classList.remove('hl'));
  const r = document.getElementById('row-' + id);
  if (r) { r.classList.add('hl'); r.scrollIntoView({behavior:'smooth', block:'nearest'}); }
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
    lastBinsData = data.bins;
    renderMap(data.bins);
    renderList(data.bins);
    renderStats(data.bins);
    checkCommercialAlerts(data.bins);
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

// ── Pick Up trigger ───────────────────────────
async function triggerPick(event, binId, binName) {
  event.stopPropagation();
  const btn = document.getElementById('pick-btn-' + binId);
  if (!btn || btn.disabled) return;
  btn.disabled = true;
  btn.textContent = 'Sending...';

  try {
    const res = await fetch('/api/pick', {
      method: 'POST',
      headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
      body: 'bin=' + binId
    });
    if (res.ok) {
      // Phase 1: servo opening (0-4s)
      btn.textContent = '↗ Servo open...';
      btn.style.background = '#1565c0';   // blue

      // Phase 2: arm about to move (4-9s)
      setTimeout(() => {
        if (btn && btn.disabled) {
          btn.textContent = '🦾 Arm moving...';
          btn.style.background = '#e65100';   // orange
        }
      }, 4000);

      // Phase 3: done (9s+)
      setTimeout(() => {
        if (btn) {
          btn.disabled = false;
          btn.textContent = '🦘 Pick Up';
          btn.style.background = '';
        }
      }, 9000);

    } else {
      throw new Error('Server error ' + res.status);
    }
  } catch (e) {
    btn.disabled = false;
    btn.textContent = '🦘 Pick Up';
    btn.style.background = '';
    alert('Could not open ' + binName + '.\n' + e.message);
  }
}
// ── Commercial alerts ────────────────────────
function isBinScheduled(binId) {
  return DAYS.some(d => schedule[d.key].includes(binId));
}

function checkCommercialAlerts(bins) {
  const banner     = document.getElementById('alert-banner');
  const activeKeys = new Set();

  commercials.forEach(binId => {
    const bin = bins.find(b => b.id === binId);
    if (!bin || !bin.ok || bin.fill < 90 || isBinScheduled(binId)) return;
    activeKeys.add('comm-full-' + binId);
  });

  Array.from(banner.children).forEach(el => {
    if (!activeKeys.has(el.dataset.alertKey)) el.remove();
  });

  activeKeys.forEach(key => {
    if (dismissedAlerts.has(key)) return;
    if (document.querySelector(`[data-alert-key="${key}"]`)) return;
    const binId  = parseInt(key.replace('comm-full-', ''));
    const bin    = bins.find(b => b.id === binId);
    const binObj = ALL_BINS.find(b => b.id === binId);
    if (!bin || !binObj) return;
    const el = document.createElement('div');
    el.className = 'alert-item'; el.dataset.alertKey = key;
    el.innerHTML = `
      <div class="alert-icon">&#128721;</div>
      <div class="alert-body">
        <div class="alert-title">Commercial bin full \u2014 Immediate action needed!</div>
        <div class="alert-msg"><strong>${binObj.name}</strong> is <strong>${bin.fill}% full</strong> and has no scheduled collection.</div>
      </div>
      <button class="alert-close" title="Dismiss" onclick="dismissAlert('${key}',this.closest('.alert-item'))">&#x2715;</button>`;
    banner.appendChild(el);
  });
}

function dismissAlert(key, el) {
  dismissedAlerts.add(key);
  el.style.transition = 'opacity .2s'; el.style.opacity = '0';
  setTimeout(() => el.remove(), 200);
}

// ── Schedule modal ────────────────────────────
function getAssignedDay(binId) {
  for (const d of DAYS) { if (schedule[d.key].includes(binId)) return d.key; }
  return null;
}

function totalAssigned() {
  const s = new Set();
  DAYS.forEach(d => schedule[d.key].forEach(id => s.add(id)));
  return s.size;
}

function switchTab(tab) {
  activeTab = tab;
  document.getElementById('tab-sched').classList.toggle('active', tab === 'sched');
  document.getElementById('tab-comm').classList.toggle('active',  tab === 'comm');
  document.getElementById('tab-sched-btn').classList.toggle('active', tab === 'sched');
  document.getElementById('tab-comm-btn').classList.toggle('active',  tab === 'comm');
  const clearBtn = document.getElementById('ftr-clear-btn');
  const saveBtn  = document.getElementById('save-btn');
  if (tab === 'comm') {
    clearBtn.textContent = 'Clear commercial'; clearBtn.onclick = clearAllComm;
    saveBtn.textContent  = 'Save commercial';  saveBtn.onclick  = saveCommercials;
  } else {
    clearBtn.textContent = 'Clear all';       clearBtn.onclick = clearAllSched;
    saveBtn.textContent  = 'Save schedule';   saveBtn.onclick  = saveSchedule;
  }
  updateFooterNote();
}

function updateFooterNote() {
  if (activeTab === 'comm') {
    const n = commercials.length;
    document.getElementById('modal-ftr-note').textContent = n + ' commercial bin' + (n !== 1 ? 's' : '') + ' selected';
  } else {
    const n = totalAssigned();
    document.getElementById('modal-ftr-note').textContent = n + ' bin' + (n !== 1 ? 's' : '') + ' assigned';
  }
}

async function openSchedModal() {
  try {
    const res = await fetch('/api/schedule');
    if (res.ok) {
      const data  = await res.json();
      schedule.mwf = data.mwf || [];
      schedule.tth = data.tth || [];
      schedule.fs  = data.fs  || [];
      commercials  = data.commercials || [];
    }
  } catch (_) {}
  switchTab('sched');
  renderModal();
  renderCommTab();
  document.getElementById('sched-overlay').classList.add('open');
}

function closeSchedModal() {
  document.getElementById('sched-overlay').classList.remove('open');
  closeDD();
}

function overlayClick(e) {
  if (e.target === document.getElementById('sched-overlay')) closeSchedModal();
}

function assignBin(binId, dayKey) {
  DAYS.forEach(d => { schedule[d.key] = schedule[d.key].filter(id => id !== binId); });
  if (!schedule[dayKey].includes(binId)) schedule[dayKey].push(binId);
  closeDD(); renderModal();
}

function removeChip(binId, dayKey) {
  schedule[dayKey] = schedule[dayKey].filter(id => id !== binId);
  renderModal();
}

function clearDay(dayKey)   { schedule[dayKey] = []; renderModal(); }
function clearAllSched()    { schedule = {mwf:[], tth:[], fs:[]}; renderModal(); }

function toggleCommercial(binId) {
  const idx = commercials.indexOf(binId);
  if (idx === -1) commercials.push(binId); else commercials.splice(idx, 1);
  renderCommTab(); updateCommBadge(); updateFooterNote();
}
function clearAllComm() { commercials = []; renderCommTab(); updateCommBadge(); updateFooterNote(); }

function updateCommBadge() {
  const badge = document.getElementById('comm-count-badge');
  badge.textContent    = commercials.length;
  badge.style.display  = commercials.length > 0 ? 'inline-block' : 'none';
}

function toggleDD(binId, event) {
  event.stopPropagation();
  if (activeDD === binId) { closeDD(); return; }
  closeDD(); activeDD = binId;
  const el = document.getElementById('dd-' + binId);
  if (el) el.style.display = 'block';
}
function closeDD() {
  if (activeDD !== null) { const el = document.getElementById('dd-' + activeDD); if (el) el.style.display = 'none'; }
  activeDD = null;
}
document.addEventListener('click', closeDD);

function renderModal() { renderModalBinList(); renderModalSchedGroups(); updateFooterNote(); }

function renderModalBinList() {
  const el = document.getElementById('modal-bin-list');
  el.innerHTML = '';

  ['a','b'].forEach(zone => {
    const zoneBins = ALL_BINS.filter(b => b.zone === zone);
    const lbl = document.createElement('div');
    lbl.className = 'panel-zone-lbl';
    lbl.textContent = 'Zone ' + zone.toUpperCase();
    el.appendChild(lbl);

    zoneBins.forEach(bin => {
      const assigned = getAssignedDay(bin.id);
      const dayInfo  = assigned ? DAYS.find(d => d.key === assigned) : null;
      const isComm   = commercials.includes(bin.id);
      const row      = document.createElement('div');
      row.className  = 'bin-item';
      row.innerHTML  = `
        <div class="bin-item-left">
          <span class="bi-dot" style="background:#1d9e75"></span>
          <span class="bi-name">${bin.name}</span>
          ${isComm   ? `<span class="bi-badge bi-badge-comm">COMM</span>` : ''}
          ${assigned ? `<span class="bi-badge ${dayInfo.badgeCls}">${dayInfo.badge}</span>` : ''}
        </div>
        <div class="add-wrap" id="wrap-${bin.id}">
          <button class="add-btn" onclick="toggleDD(${bin.id},event)">${assigned ? 'Move' : '+ Add'}</button>
          <div class="dd-menu" id="dd-${bin.id}" style="display:none" onclick="event.stopPropagation()">
            ${DAYS.map(d => `<button class="dd-item" onclick="assignBin(${bin.id},'${d.key}')">
              <span class="dd-item-badge ${d.badgeCls}">${d.badge}</span>${d.label}
            </button>`).join('')}
          </div>
        </div>`;
      el.appendChild(row);
    });
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
          const bin    = ALL_BINS.find(b => b.id === id);
          const isComm = commercials.includes(id);
          if (!bin) return '';
          return `<span class="chip">
            <span class="chip-dot" style="background:#1d9e75"></span>
            ${bin.name}${isComm ? ' <span style="font-size:9px;color:#b71c1c;">\u2605</span>' : ''}
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

function renderCommTab() {
  const el = document.getElementById('comm-zone-list');
  el.innerHTML = '';

  ['a','b'].forEach(zone => {
    const zoneBins  = ALL_BINS.filter(b => b.zone === zone);
    const zoneDot   = zone === 'a' ? '#1d9e75' : '#378add';
    const zoneLabel = zone === 'a' ? 'Zone A — 4 Bins' : 'Zone B — 4 Bins';
    const gridId    = 'comm-grid-' + zone;
    const selCount  = zoneBins.filter(b => commercials.includes(b.id)).length;

    const card = document.createElement('div');
    card.className = 'comm-zone-card';
    card.innerHTML = `
      <div class="comm-zone-hdr">
        <div class="comm-zone-title">
          <span class="comm-zone-dot" style="background:${zoneDot}"></span>
          ${zoneLabel}
        </div>
        <span class="comm-zone-sub">${selCount} commercial</span>
      </div>
      <div class="comm-grid" id="${gridId}"></div>
      <div class="comm-count-note">Click a bin to toggle commercial status</div>`;
    el.appendChild(card);

    const grid = card.querySelector('#' + gridId);
    zoneBins.forEach(bin => {
      const isComm   = commercials.includes(bin.id);
      const liveData = lastBinsData.find(b => b.id === bin.id);
      const fill     = liveData ? liveData.fill : 0;
      const bcard    = document.createElement('div');
      bcard.className = 'comm-bin-card' + (isComm ? ' selected' : '');
      bcard.onclick   = () => toggleCommercial(bin.id);
      bcard.innerHTML = `
        <div class="comm-check">
          <svg width="8" height="8" viewBox="0 0 8 8" fill="none">
            <path d="M1.5 4L3.2 5.8L6.5 2.2" stroke="#fff" stroke-width="1.3" stroke-linecap="round" stroke-linejoin="round"/>
          </svg>
        </div>
        <div class="comm-bin-name">${bin.name}</div>
        <div class="comm-bin-fill">${fill}% full</div>`;
      grid.appendChild(bcard);
    });
  });

  updateCommBadge();
}

async function saveSchedule() {
  const btn = document.getElementById('save-btn');
  btn.disabled = true; btn.textContent = 'Saving...';
  try {
    const res = await fetch('/api/schedule', {method:'POST', headers:{'Content-Type':'application/json'}, body:JSON.stringify({...schedule, commercials})});
    if (res.ok) {
      btn.textContent = 'Saved!'; dismissedAlerts.clear();
      setTimeout(() => { btn.disabled = false; btn.textContent = 'Save schedule'; closeSchedModal(); }, 900);
    } else throw new Error();
  } catch (_) { btn.disabled = false; btn.textContent = 'Save schedule'; alert('Could not save. Check connection.'); }
}

async function saveCommercials() {
  const btn = document.getElementById('save-btn');
  btn.disabled = true; btn.textContent = 'Saving...';
  try {
    const res = await fetch('/api/schedule', {method:'POST', headers:{'Content-Type':'application/json'}, body:JSON.stringify({...schedule, commercials})});
    if (res.ok) {
      btn.textContent = 'Saved!'; dismissedAlerts.clear();
      setTimeout(() => { btn.disabled = false; btn.textContent = 'Save commercial'; closeSchedModal(); }, 900);
    } else throw new Error();
  } catch (_) { btn.disabled = false; btn.textContent = 'Save commercial'; alert('Could not save. Check connection.'); }
}
</script>
</body>
</html>
)rawhtml";
}
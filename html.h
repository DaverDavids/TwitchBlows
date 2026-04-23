// html.h
#pragma once

const char INDEX_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en" data-theme="dark">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>TwitchBlows</title>
  <link rel="preconnect" href="https://fonts.googleapis.com">
  <link rel="preconnect" href="https://fonts.gstatic.com" crossorigin>
  <link href="https://fonts.googleapis.com/css2?family=JetBrains+Mono:wght@400;600;700&family=Inter:wght@400;500;600&display=swap" rel="stylesheet">
  <style>
    *, *::before, *::after { box-sizing: border-box; margin: 0; padding: 0; }

    :root {
      --bg:        #0f1117;
      --surface:   #171b24;
      --surface-2: #1e2330;
      --border:    rgba(255,255,255,0.08);
      --text:      #e2e8f0;
      --muted:     #64748b;
      --faint:     #334155;
      --accent:    #e94560;
      --accent-dim:#e9456033;
      --green:     #22c55e;
      --red:       #ef4444;
      --amber:     #f59e0b;
      --amber-dim: #f59e0b22;
      --radius:    10px;
      --font-mono: 'JetBrains Mono', 'Courier New', monospace;
      --font-ui:   'Inter', sans-serif;
      --transition: 180ms cubic-bezier(0.16,1,0.3,1);
    }

    html, body {
      min-height: 100dvh;
      background: var(--bg);
      color: var(--text);
      font-family: var(--font-ui);
      font-size: 16px;
      -webkit-font-smoothing: antialiased;
    }

    body {
      display: flex;
      flex-direction: column;
      align-items: center;
      padding: 2rem 1rem 3rem;
      gap: 1.5rem;
    }

    /* ── Header ── */
    header {
      display: flex;
      align-items: center;
      gap: 0.75rem;
      width: 100%;
      max-width: 560px;
    }

    .logo-mark { width: 36px; height: 36px; flex-shrink: 0; }

    h1 {
      font-family: var(--font-mono);
      font-size: 1.35rem;
      font-weight: 700;
      letter-spacing: -0.02em;
      color: var(--accent);
    }

    h1 span { color: var(--text); }

    .header-right { margin-left: auto; display: flex; align-items: center; gap: 0.75rem; }

    .twitch-dot {
      display: flex;
      align-items: center;
      gap: 0.35rem;
      font-size: 0.72rem;
      font-weight: 600;
      color: var(--muted);
      font-family: var(--font-mono);
    }

    .twitch-dot .dot {
      width: 8px; height: 8px;
      border-radius: 50%;
      background: var(--faint);
      transition: background var(--transition), box-shadow var(--transition);
    }

    .twitch-dot.connected .dot { background: var(--green); box-shadow: 0 0 6px var(--green); }
    .twitch-dot.connected { color: var(--green); }

    .cfg-btn {
      padding: 0.4rem 0.85rem;
      background: var(--surface-2);
      border: 1px solid var(--border);
      border-radius: 6px;
      color: var(--muted);
      font-size: 0.8rem;
      font-weight: 600;
      font-family: var(--font-ui);
      cursor: pointer;
      transition: background var(--transition), color var(--transition), border-color var(--transition);
    }

    .cfg-btn:hover { background: var(--surface); border-color: var(--accent); color: var(--accent); }

    /* ── Mode bar ── */
    .mode-bar {
      width: 100%; max-width: 560px;
      background: var(--surface);
      border: 1px solid var(--border);
      border-radius: var(--radius);
      padding: 0.75rem 1rem;
      display: flex;
      align-items: center;
      gap: 1rem;
      flex-wrap: wrap;
    }

    .mode-label {
      font-size: 0.75rem;
      font-weight: 600;
      text-transform: uppercase;
      letter-spacing: 0.08em;
      color: var(--muted);
      white-space: nowrap;
    }

    .mode-toggle {
      display: flex;
      background: var(--surface-2);
      border: 1px solid var(--border);
      border-radius: 6px;
      overflow: hidden;
    }

    .mode-toggle button {
      padding: 0.35rem 0.85rem;
      font-size: 0.8rem;
      font-weight: 600;
      font-family: var(--font-ui);
      background: none;
      border: none;
      color: var(--muted);
      cursor: pointer;
      transition: background var(--transition), color var(--transition);
      white-space: nowrap;
    }

    .mode-toggle button.active { background: var(--accent); color: #fff; }

    .pulse-row {
      display: flex;
      align-items: center;
      gap: 0.5rem;
      margin-left: auto;
    }

    .pulse-row label {
      font-size: 0.8rem;
      color: var(--muted);
      font-weight: 500;
      white-space: nowrap;
    }

    .pulse-row input[type="number"] {
      width: 90px;
      padding: 0.35rem 0.5rem;
      background: var(--surface-2);
      border: 1px solid var(--border);
      border-radius: 6px;
      color: var(--text);
      font-family: var(--font-mono);
      font-size: 0.85rem;
      font-weight: 600;
      text-align: center;
      appearance: textfield;
      -moz-appearance: textfield;
      transition: border-color var(--transition);
    }

    .pulse-row input[type="number"]:focus { outline: none; border-color: var(--accent); }
    .pulse-row input::-webkit-outer-spin-button,
    .pulse-row input::-webkit-inner-spin-button { -webkit-appearance: none; }
    .pulse-row .unit { font-size: 0.75rem; color: var(--muted); }

    /* ── Output grid ── */
    .grid {
      display: grid;
      grid-template-columns: repeat(4, 1fr);
      gap: 0.75rem;
      width: 100%;
      max-width: 560px;
    }

    .output-btn {
      position: relative;
      display: flex;
      flex-direction: column;
      align-items: center;
      justify-content: center;
      gap: 0.25rem;
      padding: 1.1rem 0.5rem 0.85rem;
      background: var(--surface);
      border: 1.5px solid var(--border);
      border-radius: var(--radius);
      color: var(--muted);
      cursor: pointer;
      font-family: var(--font-mono);
      font-weight: 700;
      font-size: 1rem;
      transition: background var(--transition), border-color var(--transition),
                  color var(--transition), box-shadow var(--transition), transform 80ms ease;
      user-select: none;
      -webkit-tap-highlight-color: transparent;
    }

    .output-btn:active:not(.dead) { transform: scale(0.96); }
    .output-btn .q-label { font-size: 1.05rem; }
    .output-btn .q-sub {
      font-size: 0.6rem;
      font-family: var(--font-ui);
      font-weight: 500;
      text-transform: uppercase;
      letter-spacing: 0.06em;
      opacity: 0.7;
    }

    .output-btn::before {
      content: '';
      position: absolute;
      top: 8px; right: 10px;
      width: 7px; height: 7px;
      border-radius: 50%;
      background: var(--faint);
      transition: background var(--transition), box-shadow var(--transition);
    }

    .output-btn.active-toggle {
      background: var(--accent-dim);
      border-color: var(--accent);
      color: var(--accent);
      box-shadow: 0 0 16px var(--accent-dim);
    }
    .output-btn.active-toggle::before { background: var(--accent); box-shadow: 0 0 6px var(--accent); }

    .output-btn.active-pulse {
      background: var(--amber-dim);
      border-color: var(--amber);
      color: var(--amber);
      box-shadow: 0 0 16px var(--amber-dim);
      animation: pulse-glow 0.7s ease-in-out infinite alternate;
    }
    .output-btn.active-pulse::before { background: var(--amber); box-shadow: 0 0 6px var(--amber); }

    .output-btn.dead {
      background: var(--surface);
      border-color: var(--faint);
      color: var(--faint);
      cursor: not-allowed;
      opacity: 0.6;
    }
    .output-btn.dead::before { background: var(--red); opacity: 0.5; }

    .output-btn.next::after {
      content: '\\25B6';
      position: absolute;
      top: 6px;
      left: 8px;
      font-size: 0.6rem;
      color: var(--green);
      font-weight: 700;
    }

    @keyframes pulse-glow {
      from { box-shadow: 0 0 8px var(--amber-dim); }
      to   { box-shadow: 0 0 22px var(--amber); }
    }

    .pulse-bar {
      position: absolute;
      bottom: 0; left: 0;
      height: 3px;
      background: var(--amber);
      border-radius: 0 0 var(--radius) var(--radius);
      width: 100%;
      transform-origin: left;
      transform: scaleX(1);
    }

    /* ── Bottom row ── */
    .bottom-row {
      display: flex;
      gap: 0.75rem;
      width: 100%;
      max-width: 560px;
    }

    .all-off-btn {
      flex: 1;
      padding: 0.8rem;
      background: var(--surface);
      border: 1.5px solid var(--border);
      border-radius: var(--radius);
      color: var(--muted);
      font-size: 0.85rem;
      font-weight: 600;
      font-family: var(--font-ui);
      cursor: pointer;
      transition: background var(--transition), border-color var(--transition), color var(--transition);
    }

    .all-off-btn:hover { background: var(--surface-2); border-color: var(--accent); color: var(--accent); }

    .reset-used-btn {
      flex: 1;
      padding: 0.8rem;
      background: var(--surface);
      border: 1.5px solid var(--border);
      border-radius: var(--radius);
      color: var(--muted);
      font-size: 0.85rem;
      font-weight: 600;
      font-family: var(--font-ui);
      cursor: pointer;
      transition: background var(--transition), border-color var(--transition), color var(--transition);
    }

    .reset-used-btn:hover { background: var(--surface-2); border-color: var(--green); color: var(--green); }

    /* ── Status bar ── */
    .status-bar {
      width: 100%; max-width: 560px;
      background: var(--surface);
      border: 1px solid var(--border);
      border-radius: var(--radius);
      padding: 0.6rem 1rem;
      display: flex;
      align-items: center;
      gap: 0.6rem;
    }

    .status-dot {
      width: 8px; height: 8px;
      border-radius: 50%;
      background: var(--faint);
      flex-shrink: 0;
      transition: background var(--transition);
    }

    .status-dot.on-toggle { background: var(--accent); box-shadow: 0 0 6px var(--accent); }
    .status-dot.on-pulse  { background: var(--amber);  box-shadow: 0 0 6px var(--amber); }

    #status-text { font-size: 0.8rem; color: var(--muted); font-family: var(--font-mono); }

    /* ── ADC bar ── */
    .adc-bar {
      width: 100%; max-width: 560px;
      background: var(--surface);
      border: 1px solid var(--border);
      border-radius: var(--radius);
      padding: 0.5rem 1rem;
      display: flex;
      align-items: center;
      gap: 0.5rem;
      font-family: var(--font-mono);
      font-size: 0.85rem;
    }

    .adc-label { color: var(--muted); font-weight: 600; }

    .adc-value { color: var(--green); font-weight: 700; }

    /* ── Console ── */
    .console {
      width: 100%; max-width: 560px;
      background: #0a0c10;
      border: 1px solid var(--border);
      border-radius: var(--radius);
      padding: 0.75rem;
      font-family: var(--font-mono);
      font-size: 0.72rem;
      color: #7ee787;
      height: 200px;
      overflow-y: auto;
      white-space: pre-wrap;
      word-break: break-all;
    }

    .console-header {
      display: flex;
      align-items: center;
      justify-content: space-between;
      width: 100%; max-width: 560px;
      font-size: 0.75rem;
      color: var(--muted);
      font-weight: 600;
      text-transform: uppercase;
      letter-spacing: 0.07em;
    }

    .console-header button {
      font-size: 0.7rem;
      padding: 2px 8px;
      background: var(--surface-2);
      border: 1px solid var(--border);
      border-radius: 4px;
      color: var(--muted);
      cursor: pointer;
    }

    .console-header button:hover {
      background: var(--surface);
      color: var(--text);
    }

    /* ── Config section ── */
    #config-section {
      width: 100%; max-width: 560px;
      background: var(--surface);
      border: 1px solid var(--border);
      border-radius: var(--radius);
      padding: 1.25rem;
      display: none;
      flex-direction: column;
      gap: 1rem;
    }

    #config-section.visible { display: flex; }

    .cfg-title {
      font-size: 1rem;
      font-weight: 700;
      font-family: var(--font-mono);
      color: var(--accent);
      border-bottom: 1px solid var(--border);
      padding-bottom: 0.5rem;
      margin-bottom: 0.25rem;
    }

    .cfg-row {
      display: flex;
      flex-direction: column;
      gap: 0.3rem;
    }

    .cfg-row label {
      font-size: 0.75rem;
      font-weight: 600;
      text-transform: uppercase;
      letter-spacing: 0.06em;
      color: var(--muted);
    }

    .cfg-row input[type="text"],
    .cfg-row input[type="password"],
    .cfg-row input[type="number"] {
      padding: 0.5rem 0.75rem;
      background: var(--surface-2);
      border: 1px solid var(--border);
      border-radius: 6px;
      color: var(--text);
      font-family: var(--font-mono);
      font-size: 0.85rem;
      transition: border-color var(--transition);
    }

    .cfg-row input:focus { outline: none; border-color: var(--accent); }
    .cfg-row input::-webkit-outer-spin-button,
    .cfg-row input::-webkit-inner-spin-button { -webkit-appearance: none; }

    .cfg-row-inline {
      display: grid;
      grid-template-columns: 1fr 1fr;
      gap: 0.75rem;
    }

    .cfg-checkboxes {
      display: flex;
      flex-wrap: wrap;
      gap: 0.5rem;
    }

    .cfg-checkboxes label {
      display: flex;
      align-items: center;
      gap: 0.35rem;
      font-size: 0.8rem;
      color: var(--muted);
      cursor: pointer;
      padding: 0.3rem 0.6rem;
      background: var(--surface-2);
      border: 1px solid var(--border);
      border-radius: 4px;
      transition: border-color var(--transition), color var(--transition);
      text-transform: none;
      letter-spacing: 0;
      font-weight: 500;
    }

    .cfg-checkboxes label:has(input:checked) { border-color: var(--accent); color: var(--accent); }
    .cfg-checkboxes input { display: none; }

    .cfg-actions {
      display: flex;
      gap: 0.75rem;
      margin-top: 0.25rem;
    }

    .cfg-save-btn {
      flex: 1;
      padding: 0.75rem;
      background: var(--accent);
      border: none;
      border-radius: var(--radius);
      color: #fff;
      font-size: 0.9rem;
      font-weight: 700;
      font-family: var(--font-ui);
      cursor: pointer;
      transition: opacity var(--transition);
    }

    .cfg-save-btn:hover { opacity: 0.85; }
    .cfg-cancel-btn {
      padding: 0.75rem 1.25rem;
      background: var(--surface-2);
      border: 1px solid var(--border);
      border-radius: var(--radius);
      color: var(--muted);
      font-size: 0.9rem;
      font-weight: 600;
      font-family: var(--font-ui);
      cursor: pointer;
      transition: background var(--transition), color var(--transition);
    }

    .cfg-cancel-btn:hover { background: var(--surface); color: var(--text); }

    @media (max-width: 420px) {
      .mode-bar { flex-direction: column; align-items: flex-start; }
      .pulse-row { margin-left: 0; }
      .cfg-row-inline { grid-template-columns: 1fr; }
    }
  </style>
</head>
<body>

<header>
  <svg class="logo-mark" viewBox="0 0 36 36" fill="none" aria-label="TwitchBlows logo" xmlns="http://www.w3.org/2000/svg">
    <rect width="36" height="36" rx="8" fill="#e94560" fill-opacity="0.12"/>
    <path d="M9 12 L18 7 L27 12 L27 24 L18 29 L9 24 Z" stroke="#e94560" stroke-width="1.8" fill="none"/>
    <circle cx="18" cy="18" r="4" fill="#e94560"/>
    <path d="M18 14 L18 7 M18 22 L18 29 M14 16 L9 12 M22 20 L27 24 M14 20 L9 24 M22 16 L27 12" stroke="#e94560" stroke-width="1.4" opacity="0.5"/>
  </svg>
  <h1><span>Twitch</span>Blows</h1>
  <div class="header-right">
    <div class="twitch-dot" id="twitch-dot">
      <div class="dot"></div>
      <span>Twitch</span>
    </div>
    <button class="cfg-btn" onclick="toggleConfig()">Config</button>
  </div>
</header>

<div class="mode-bar">
  <span class="mode-label">Mode</span>
  <div class="mode-toggle">
    <button id="btn-toggle-mode" class="active" onclick="setMode('toggle')">Toggle</button>
    <button id="btn-pulse-mode"            onclick="setMode('pulse')">Pulse</button>
  </div>
  <div class="pulse-row" id="pulse-row" style="opacity:0.35; pointer-events:none;">
    <label for="pulse-ms">Duration</label>
    <input type="number" id="pulse-ms" value="500" min="10" max="30000" step="10">
    <span class="unit">ms</span>
  </div>
</div>

<div class="grid" id="grid"></div>

<div class="bottom-row">
  <button class="all-off-btn" onclick="allOff()">&#11035; ALL OFF</button>
  <button class="reset-used-btn" onclick="resetUsed()">&#8635; RESET DEAD</button>
</div>

<div class="adc-bar">
  <span class="adc-label">A0</span>
  <span class="adc-value" id="adc-curr">--</span>
  <span class="adc-label">MAX</span>
  <span class="adc-value" id="adc-max">--</span>
  <span class="adc-label">MIN</span>
  <span class="adc-value" id="adc-min">--</span>
</div>

<div class="adc-bar">
  <span class="adc-label">AMP</span>
  <span class="adc-value" id="amp-curr">--</span>
  <span class="adc-label">MAX</span>
  <span class="adc-value" id="amp-max">--</span>
  <span class="adc-label">MIN</span>
  <span class="adc-value" id="amp-min">--</span>
</div>

<div class="status-bar">
  <div class="status-dot" id="status-dot"></div>
  <span id="status-text">Loading&#8230;</span>
</div>

<div class="console-header">
  <span>&#9679; Device Console</span>
  <button onclick="clearConsole()">Clear</button>
</div>
<div class="console" id="console">Connecting...</div>

<div id="config-section">
  <div class="cfg-title">TwitchBlows Config</div>

  <div class="cfg-row">
    <label for="cfg-channel">Twitch Channel</label>
    <input type="text" id="cfg-channel" placeholder="daverdavid">
  </div>

  <div class="cfg-row">
    <label for="cfg-oauth">OAuth Token</label>
    <input type="password" id="cfg-oauth" placeholder="oauth:...">
  </div>

  <div class="cfg-row">
    <label for="cfg-nick">Bot Nickname</label>
    <input type="text" id="cfg-nick" placeholder="your_bot_username">
  </div>

  <div class="cfg-row-inline">
    <div class="cfg-row">
      <label for="cfg-bits">Bits Threshold</label>
      <input type="number" id="cfg-bits" value="100" min="1" max="1000000" step="1">
    </div>
    <div class="cfg-row">
      <label for="cfg-points-thresh">Points (redemptions)</label>
      <input type="number" id="cfg-points-thresh" value="1" min="1" max="1000" step="1">
    </div>
  </div>
  <div class="cfg-row-inline">
    <div class="cfg-row">
      <label for="cfg-subs-thresh">Subs Threshold</label>
      <input type="number" id="cfg-subs-thresh" value="1" min="1" max="1000" step="1">
    </div>
    <div class="cfg-row">
      <label for="cfg-raid-thresh">Raid Viewers</label>
      <input type="number" id="cfg-raid-thresh" value="10" min="1" max="100000" step="1">
    </div>
  </div>
  <div class="cfg-row">
    <label for="cfg-pulse">Pulse Duration (ms)</label>
    <input type="number" id="cfg-pulse" value="500" min="10" max="30000" step="10">
  </div>

  <div class="cfg-row">
    <label for="cfg-cs-delay">Current Sense Delay (ms)</label>
    <input type="number" id="cfg-cs-delay" value="10" min="1" max="500" step="1">
  </div>

  <div class="cfg-row">
    <label for="cfg-pts-filter">Points Reward ID Filter (blank = all)</label>
    <input type="text" id="cfg-pts-filter" placeholder="e.g. a1b2c3d4-e5f6-...">
  </div>

  <div class="cfg-checkboxes">
    <label><input type="checkbox" id="cb-bits"> Bits</label>
    <label><input type="checkbox" id="cb-points"> Channel Points</label>
    <label><input type="checkbox" id="cb-subs"> Subs</label>
    <label><input type="checkbox" id="cb-raids"> Raids</label>
  </div>

  <div class="cfg-actions">
    <button class="cfg-save-btn" onclick="saveCfg()">Save Config</button>
    <button class="cfg-cancel-btn" onclick="toggleConfig()">Cancel</button>
  </div>
</div>

<script>
  // ── Persistence helpers ────────────────────
  const LS_MODE = 'tb_mode';
  const LS_MS   = 'tb_pulsems';

  function savePrefs() {
    try {
      localStorage.setItem(LS_MODE, mode);
      localStorage.setItem(LS_MS,   document.getElementById('pulse-ms').value);
    } catch(e) {}
  }

  function loadPrefs() {
    try {
      const savedMode = localStorage.getItem(LS_MODE);
      const savedMs   = localStorage.getItem(LS_MS);
      if (savedMode === 'toggle' || savedMode === 'pulse') mode = savedMode;
      if (savedMs !== null) {
        const ms = parseInt(savedMs);
        if (ms >= 10 && ms <= 30000) document.getElementById('pulse-ms').value = ms;
      }
    } catch(e) {}
  }

  // ── State ──────────────────────────────────
  let mode        = 'toggle';
  let activeQ     = -1;
  let pulseTimers = {};
  let pulseBars   = {};
  let usedMask    = 0;
  let twitchConn  = false;
  let nextQ      = 0;

  // ── Build buttons ─────────────────────────
  const grid = document.getElementById('grid');
  for (let i = 0; i < 16; i++) {
    const btn = document.createElement('button');
    btn.className = 'output-btn';
    btn.id = 'btn' + i;
    btn.setAttribute('aria-label', 'Channel ' + (i+1));
    btn.innerHTML =
      '<span class="q-label">Ch' + (i+1) + '</span>' +
      '<span class="q-sub" id="sub' + i + '">&#8212;</span>';
    btn.onclick = () => handleBtnClick(i);
    grid.appendChild(btn);
  }

  loadPrefs();
  applyModeUI(mode);

  document.getElementById('pulse-ms').addEventListener('change', savePrefs);
  document.getElementById('pulse-ms').addEventListener('input',  savePrefs);

  // ── Mode switching ─────────────────────────
  function setMode(m) {
    mode = m;
    applyModeUI(m);
    savePrefs();
  }

  function applyModeUI(m) {
    document.getElementById('btn-toggle-mode').classList.toggle('active', m === 'toggle');
    document.getElementById('btn-pulse-mode').classList.toggle('active', m === 'pulse');
    const pr = document.getElementById('pulse-row');
    pr.style.opacity       = m === 'pulse' ? '1'    : '0.35';
    pr.style.pointerEvents = m === 'pulse' ? 'auto' : 'none';
  }

  // ── Button click ──────────────────────────
  function handleBtnClick(i) {
    const btn = document.getElementById('btn' + i);
    if (btn && btn.classList.contains('dead')) return;
    if (mode === 'toggle') {
      sendSet(activeQ === i ? -1 : i);
    } else {
      const ms = Math.max(10, parseInt(document.getElementById('pulse-ms').value) || 500);
      sendPulse(i, ms);
    }
  }

  // ── Toggle set ────────────────────────────
  function sendSet(q) {
    fetch('/set?q=' + q)
      .then(r => r.json())
      .then(data => { activeQ = data.active; updateToggleUI(); updateStatus(); })
      .catch(() => setStatusText('Error contacting device'));
  }

  // ── Pulse ─────────────────────────────────
  function sendPulse(q, ms) {
    cancelPulse(q);
    fetch('/pulse?q=' + q + '&ms=' + ms)
      .then(r => r.json())
      .then(data => { if (data.ok) startPulseUI(q, ms); })
      .catch(() => setStatusText('Error contacting device'));
  }

  function cancelPulse(q) {
    if (pulseTimers[q]) { clearTimeout(pulseTimers[q]); delete pulseTimers[q]; }
    if (pulseBars[q])   { cancelAnimationFrame(pulseBars[q].raf); delete pulseBars[q]; }
    const btn = document.getElementById('btn' + q);
    if (btn) {
      btn.classList.remove('active-pulse');
      const bar = btn.querySelector('.pulse-bar');
      if (bar) bar.remove();
      document.getElementById('sub' + q).textContent = '\u2014';
    }
  }

  function startPulseUI(q, ms) {
    const btn = document.getElementById('btn' + q);
    btn.classList.add('active-pulse');
    let bar = btn.querySelector('.pulse-bar');
    if (!bar) { bar = document.createElement('div'); bar.className = 'pulse-bar'; btn.appendChild(bar); }
    const start = performance.now();
    function tick(now) {
      const elapsed  = now - start;
      const progress = Math.min(elapsed / ms, 1);
      bar.style.transform = 'scaleX(' + (1 - progress) + ')';
      const rem = Math.max(0, (ms - elapsed) / 1000);
      document.getElementById('sub' + q).textContent = rem.toFixed(1) + 's';
      if (progress < 1) pulseBars[q] = { raf: requestAnimationFrame(tick) };
    }
    pulseBars[q] = { raf: requestAnimationFrame(tick) };
    updateStatusPulse(q, ms);
    pulseTimers[q] = setTimeout(() => { cancelPulse(q); updateStatus(); }, ms + 300);
  }

  // ── All off & reset used ─────────────────────────────
  function allOff() {
    for (let i = 0; i < 16; i++) cancelPulse(i);
    sendSet(-1);
  }

  function resetUsed() {
    fetch('/resetused')
      .then(r => r.json())
      .then(data => {
        if (data.ok) {
          usedMask = 0;
          updateDeadUI();
          setStatusText('Dead outputs reset');
        }
      })
      .catch(() => {});
  }

  // ── Dead output UI ─────────────────────────
  function updateDeadUI() {
    for (let i = 0; i < 16; i++) {
      const btn = document.getElementById('btn' + i);
      if (btn) {
        btn.classList.toggle('dead', (usedMask & (1 << i)) !== 0);
      }
    }
  }

  // ── Next output indicator ─────────────────────
  function updateNextUI() {
    for (let i = 0; i < 16; i++) {
      const btn = document.getElementById('btn' + i);
      if (btn) {
        btn.classList.toggle('next', i === nextQ && !btn.classList.contains('dead'));
      }
    }
  }

  // ── UI updates ────────────────────────────
  function updateToggleUI() {
    for (let i = 0; i < 16; i++) {
      const btn = document.getElementById('btn' + i);
      btn.classList.remove('active-toggle');
      if (!pulseTimers[i]) document.getElementById('sub' + i).textContent = '\u2014';
      if (i === activeQ) { btn.classList.add('active-toggle'); document.getElementById('sub' + i).textContent = 'ON'; }
    }
  }

  function updateStatus() {
    if (Object.keys(pulseTimers).length > 0) return;
    const dot = document.getElementById('status-dot');
    dot.className = 'status-dot' + (activeQ >= 0 ? ' on-toggle' : '');
    document.getElementById('status-text').textContent =
      activeQ === -1 ? 'All outputs OFF' : 'Q' + activeQ + ' ON (toggle)';
  }

  function updateStatusPulse(q, ms) {
    document.getElementById('status-dot').className = 'status-dot on-pulse';
    document.getElementById('status-text').textContent = 'Q' + q + ' pulsing ' + ms + 'ms';
  }

  function setStatusText(msg) { document.getElementById('status-text').textContent = msg; }

  // ── State poll ─────────────────────────────
  function pollState() {
    fetch('/state')
      .then(r => r.json())
      .then(data => {
        activeQ   = data.active;
        usedMask  = data.used || 0;
        twitchConn = data.twitch === true || data.twitch === 'true';
        nextQ    = data.nextQ || 0;

        // Update ADC readings (now in millivolts)
        if (data.adcCurr !== undefined) {
          document.getElementById('adc-curr').textContent = data.adcCurr + 'mV';
        }
        if (data.adcMax !== undefined) {
          document.getElementById('adc-max').textContent = data.adcMax + 'mV';
        }
        if (data.adcMin !== undefined) {
          document.getElementById('adc-min').textContent = data.adcMin + 'mV';
        }
        if (data.ampCurr !== undefined) {
          document.getElementById('amp-curr').textContent = data.ampCurr + 'A';
        }
        if (data.ampMax !== undefined) {
          document.getElementById('amp-max').textContent = data.ampMax + 'A';
        }
        if (data.ampMin !== undefined) {
          document.getElementById('amp-min').textContent = data.ampMin + 'A';
        }

        // Update peak ADC values in button sub-labels
        if (data.peaks) {
          for (let i = 0; i < 16; i++) {
            const sub = document.getElementById('sub' + i);
            if (sub && data.peaks[i] > 0 && !sub.textContent.includes('ON')) {
              sub.textContent = data.peaks[i] + 'mA';
            }
          }
        }

        updateToggleUI();
        updateDeadUI();
        updateNextUI();
        updateStatus();

        const td = document.getElementById('twitch-dot');
        td.className = 'twitch-dot' + (twitchConn ? ' connected' : '');
      })
      .catch(() => {});
  }

  // ── Config ──────────────────────────────
  function toggleConfig() {
    const sec = document.getElementById('config-section');
    const visible = sec.classList.toggle('visible');
    if (visible) {
      fetch('/getcfg')
        .then(r => r.json())
        .then(data => {
          document.getElementById('cfg-channel').value = data.channel || '';
          document.getElementById('cfg-bits').value   = data.bitsThreshold || 100;
          document.getElementById('cfg-points-thresh').value = data.pointsThreshold || 1;
          document.getElementById('cfg-subs-thresh').value   = data.subsThreshold || 1;
          document.getElementById('cfg-raid-thresh').value   = data.raidThreshold || 10;
          document.getElementById('cfg-pulse').value  = data.pulseDurMs || 500;
          document.getElementById('cfg-cs-delay').value = data.csDelayMs || 10;
          document.getElementById('cfg-pts-filter').value = data.ptsFilter || '';
          document.getElementById('cb-bits').checked   = data.evBits === true || data.evBits === 'true';
          document.getElementById('cb-points').checked = data.evPoints === true || data.evPoints === 'true';
          document.getElementById('cb-subs').checked   = data.evSubs === true || data.evSubs === 'true';
          document.getElementById('cb-raids').checked  = data.evRaids === true || data.evRaids === 'true';
          document.getElementById('cfg-oauth').value  = '';
          document.getElementById('cfg-nick').value   = '';
        })
        .catch(() => {});
    }
  }

  function saveCfg() {
    const params = new URLSearchParams();
    const ch = document.getElementById('cfg-channel').value.trim();
    const bits = document.getElementById('cfg-bits').value;
    const pointsThresh = document.getElementById('cfg-points-thresh').value;
    const subsThresh = document.getElementById('cfg-subs-thresh').value;
    const raidThresh = document.getElementById('cfg-raid-thresh').value;
    const pulse = document.getElementById('cfg-pulse').value;
    const csDelay = document.getElementById('cfg-cs-delay').value;
    const ptsFilter = document.getElementById('cfg-pts-filter').value.trim();
    const oauth = document.getElementById('cfg-oauth').value.trim();
    const nick = document.getElementById('cfg-nick').value.trim();

    if (ch) params.append('channel', ch);
    if (bits) params.append('bits_threshold', bits);
    if (pointsThresh) params.append('points_threshold', pointsThresh);
    if (subsThresh) params.append('subs_threshold', subsThresh);
    if (raidThresh) params.append('raid_threshold', raidThresh);
    if (pulse) params.append('pulse_ms', pulse);
    if (csDelay) params.append('cs_delay_ms', csDelay);
    if (ptsFilter) params.append('pts_filter', ptsFilter);
    params.append('ev_bits', document.getElementById('cb-bits').checked ? '1' : '0');
    params.append('ev_points', document.getElementById('cb-points').checked ? '1' : '0');
    params.append('ev_subs', document.getElementById('cb-subs').checked ? '1' : '0');
    params.append('ev_raids', document.getElementById('cb-raids').checked ? '1' : '0');
    if (oauth) params.append('oauth', oauth);
    if (nick) params.append('nick', nick);

    fetch('/savecfg', { method: 'POST', body: params })
      .then(r => r.json())
      .then(data => {
        if (data.ok) {
          setStatusText('Config saved');
          toggleConfig();
        }
      })
      .catch(() => setStatusText('Error saving config'));
  }

  // ── Console polling ───────────────────────────
  let consoleSeen = '';
  function pollLog() {
    fetch('/log')
      .then(r => r.text())
      .then(txt => {
        if (txt !== consoleSeen) {
          consoleSeen = txt;
          const el = document.getElementById('console');
          el.textContent = txt;
          el.scrollTop = el.scrollHeight;
        }
      })
      .catch(() => {});
  }
  function clearConsole() {
    document.getElementById('console').textContent = '';
    consoleSeen = '';
  }

  // ── Init ──────────────────────────────────
  pollState();
  pollLog();
  setInterval(pollState, 3000);
  setInterval(pollLog, 1000);
</script>
</body>
</html>
)rawliteral";

// Captive portal / WiFi setup page
const char PORTAL_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>TwitchBlows WiFi Setup</title>
  <style>
    *, *::before, *::after { box-sizing: border-box; margin: 0; padding: 0; }
    body { font-family:sans-serif; background:#0f1117; color:#e2e8f0; display:flex; flex-direction:column; align-items:center; padding:2rem; gap:1rem; }
    h1   { color:#e94560; font-size:1.4rem; }
    form { display:flex; flex-direction:column; gap:.8rem; width:100%; max-width:340px; margin-top:1rem; }
    input { padding:.7rem; border-radius:6px; border:1px solid rgba(255,255,255,0.1); background:#171b24; color:#e2e8f0; font-size:1rem; }
    input:focus { outline: 2px solid #e94560; }
    input[type=submit] { background:#e94560; border:none; cursor:pointer; font-weight:bold; }
    input[type=submit]:hover { background:#c73652; }
    p.note { font-size:.85rem; color:#64748b; text-align:center; }
  </style>
</head>
<body>
  <h1>WiFi Setup</h1>
  <p class="note">Connect to configure your network credentials.</p>
  <form action="/savewifi" method="GET">
    <input type="text"     name="ssid" placeholder="SSID"     required>
    <input type="password" name="psk"  placeholder="Password">
    <input type="submit" value="Save &amp; Connect">
  </form>
</body>
</html>
)rawliteral";

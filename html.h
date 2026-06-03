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
      --tr: 180ms cubic-bezier(0.16,1,0.3,1);
    }

    html, body { min-height: 100dvh; background: var(--bg); color: var(--text); font-family: var(--font-ui); font-size: 15px; -webkit-font-smoothing: antialiased; }

    body { display: flex; flex-direction: column; align-items: center; padding: 1.25rem 1rem 2.5rem; gap: 1rem; }

    /* ── Pages ── */
    .page { display: none; flex-direction: column; align-items: center; gap: 1rem; width: 100%; }
    .page.active { display: flex; }

    /* ── Header ── */
    header {
      display: flex; align-items: center; gap: 0.6rem;
      width: 100%; max-width: 560px;
    }
    .logo-mark { width: 30px; height: 30px; flex-shrink: 0; }
    h1 { font-family: var(--font-mono); font-size: 1.2rem; font-weight: 700; letter-spacing: -0.02em; color: var(--accent); }
    h1 span { color: var(--text); }
    .header-right { margin-left: auto; display: flex; align-items: center; gap: 0.6rem; }

    .twitch-dot { display: flex; align-items: center; gap: 0.3rem; font-size: 0.68rem; font-weight: 600; color: var(--muted); font-family: var(--font-mono); }
    .twitch-dot .dot { width: 7px; height: 7px; border-radius: 50%; background: var(--faint); transition: background var(--tr), box-shadow var(--tr); }
    .twitch-dot.connected .dot { background: var(--green); box-shadow: 0 0 5px var(--green); }
    .twitch-dot.connected { color: var(--green); }

    .nav-btn {
      padding: 0.3rem 0.7rem;
      background: var(--surface-2); border: 1px solid var(--border); border-radius: 6px;
      color: var(--muted); font-size: 0.75rem; font-weight: 600; font-family: var(--font-ui);
      cursor: pointer; transition: background var(--tr), color var(--tr), border-color var(--tr);
    }
    .nav-btn:hover { background: var(--surface); border-color: var(--accent); color: var(--accent); }
    .nav-btn.active { border-color: var(--accent); color: var(--accent); background: var(--accent-dim); }

    /* ── Mode bar ── */
    .mode-bar {
      width: 100%; max-width: 560px;
      background: var(--surface); border: 1px solid var(--border); border-radius: var(--radius);
      padding: 0.6rem 0.9rem;
      display: flex; align-items: center; gap: 0.75rem; flex-wrap: wrap;
    }
    .mode-label { font-size: 0.7rem; font-weight: 600; text-transform: uppercase; letter-spacing: 0.08em; color: var(--muted); white-space: nowrap; }
    .mode-toggle { display: flex; background: var(--surface-2); border: 1px solid var(--border); border-radius: 6px; overflow: hidden; }
    .mode-toggle button { padding: 0.3rem 0.75rem; font-size: 0.75rem; font-weight: 600; font-family: var(--font-ui); background: none; border: none; color: var(--muted); cursor: pointer; transition: background var(--tr), color var(--tr); white-space: nowrap; }
    .mode-toggle button.active { background: var(--accent); color: #fff; }
    .pulse-row { display: flex; align-items: center; gap: 0.4rem; margin-left: auto; }
    .pulse-row label { font-size: 0.72rem; color: var(--muted); font-weight: 500; white-space: nowrap; }
    .pulse-row input[type="number"] { width: 80px; padding: 0.28rem 0.45rem; background: var(--surface-2); border: 1px solid var(--border); border-radius: 6px; color: var(--text); font-family: var(--font-mono); font-size: 0.8rem; font-weight: 600; text-align: center; appearance: textfield; -moz-appearance: textfield; transition: border-color var(--tr); }
    .pulse-row input[type="number"]:focus { outline: none; border-color: var(--accent); }
    .pulse-row input::-webkit-outer-spin-button, .pulse-row input::-webkit-inner-spin-button { -webkit-appearance: none; }
    .pulse-row .unit { font-size: 0.7rem; color: var(--muted); }

    /* ── Output grid ── */
    .grid { display: grid; grid-template-columns: repeat(4, 1fr); gap: 0.6rem; width: 100%; max-width: 560px; }

    .output-btn {
      position: relative; display: flex; flex-direction: column; align-items: center; justify-content: center; gap: 0.2rem;
      padding: 0.9rem 0.4rem 0.7rem;
      background: var(--surface); border: 1.5px solid var(--border); border-radius: var(--radius);
      color: var(--muted); cursor: pointer; font-family: var(--font-mono); font-weight: 700; font-size: 0.95rem;
      transition: background var(--tr), border-color var(--tr), color var(--tr), box-shadow var(--tr), transform 80ms ease;
      user-select: none; -webkit-tap-highlight-color: transparent;
    }
    .output-btn:active:not(.dead) { transform: scale(0.96); }
    .output-btn .q-label { font-size: 0.95rem; }
    .output-btn .q-sub { font-size: 0.75rem; font-family: var(--font-ui); font-weight: 600; opacity: 0.8; }
    .output-btn::before { content: ''; position: absolute; top: 7px; right: 9px; width: 6px; height: 6px; border-radius: 50%; background: var(--faint); transition: background var(--tr), box-shadow var(--tr); }
    .output-btn.active-toggle { background: var(--accent-dim); border-color: var(--accent); color: var(--accent); box-shadow: 0 0 14px var(--accent-dim); }
    .output-btn.active-toggle::before { background: var(--accent); box-shadow: 0 0 5px var(--accent); }
    .output-btn.active-pulse { background: var(--amber-dim); border-color: var(--amber); color: var(--amber); box-shadow: 0 0 14px var(--amber-dim); animation: pulse-glow 0.7s ease-in-out infinite alternate; }
    .output-btn.active-pulse::before { background: var(--amber); box-shadow: 0 0 5px var(--amber); }
    .output-btn.dead { background: var(--surface); border-color: var(--faint); color: var(--faint); cursor: not-allowed; opacity: 0.6; }
    .output-btn.dead::before { background: var(--red); opacity: 0.5; }
    .output-btn.next::after { content: '\25B6'; position: absolute; top: 5px; left: 7px; font-size: 0.55rem; color: var(--green); font-weight: 700; }
    .output-btn .q-toggle { position: absolute; bottom: 3px; right: 5px; font-size: 0.52rem; cursor: pointer; opacity: 0.35; transition: opacity var(--tr); background: none; border: none; color: var(--muted); padding: 2px 3px; line-height: 1; }
    .output-btn .q-toggle:hover { opacity: 1; color: var(--accent); }

    @keyframes pulse-glow {
      from { box-shadow: 0 0 7px var(--amber-dim); }
      to   { box-shadow: 0 0 20px var(--amber); }
    }

    .pulse-bar { position: absolute; bottom: 0; left: 0; height: 3px; background: var(--amber); border-radius: 0 0 var(--radius) var(--radius); width: 100%; transform-origin: left; transform: scaleX(1); }

    /* ── Bottom row ── */
    .bottom-row { display: flex; gap: 0.6rem; width: 100%; max-width: 560px; }
    .action-btn {
      flex: 1; padding: 0.65rem;
      background: var(--surface); border: 1.5px solid var(--border); border-radius: var(--radius);
      color: var(--muted); font-size: 0.78rem; font-weight: 600; font-family: var(--font-ui);
      cursor: pointer; transition: background var(--tr), border-color var(--tr), color var(--tr);
    }
    .action-btn:hover { background: var(--surface-2); border-color: var(--accent); color: var(--accent); }
    .action-btn.green:hover { border-color: var(--green); color: var(--green); }

    /* ── ADC / amp bars ── */
    .adc-bar {
      width: 100%; max-width: 560px;
      background: var(--surface); border: 1px solid var(--border); border-radius: var(--radius);
      padding: 0.45rem 0.85rem;
      display: flex; align-items: center; gap: 0.5rem; flex-wrap: wrap;
    }
    .adc-label { font-size: 0.62rem; font-weight: 700; text-transform: uppercase; letter-spacing: 0.08em; color: var(--muted); font-family: var(--font-mono); }
    .adc-value { font-size: 0.78rem; font-weight: 600; font-family: var(--font-mono); color: var(--text); min-width: 5ch; }

    /* ── Status bar ── */
    .status-bar { width: 100%; max-width: 560px; background: var(--surface); border: 1px solid var(--border); border-radius: var(--radius); padding: 0.5rem 0.85rem; display: flex; align-items: center; gap: 0.5rem; }
    .status-dot { width: 7px; height: 7px; border-radius: 50%; background: var(--faint); transition: background var(--tr), box-shadow var(--tr); flex-shrink: 0; }
    .status-dot.ok   { background: var(--green); box-shadow: 0 0 5px var(--green); }
    .status-dot.warn { background: var(--amber); box-shadow: 0 0 5px var(--amber); }
    .status-dot.err  { background: var(--red);   box-shadow: 0 0 5px var(--red); }
    #status-text { font-size: 0.75rem; color: var(--muted); font-family: var(--font-mono); }

    /* ── Console ── */
    .console-header { width: 100%; max-width: 560px; display: flex; align-items: center; justify-content: space-between; padding: 0.3rem 0.85rem 0; }
    .console-header span { font-size: 0.65rem; font-weight: 700; text-transform: uppercase; letter-spacing: 0.08em; color: var(--faint); font-family: var(--font-mono); }
    .console-header button { font-size: 0.65rem; font-weight: 600; font-family: var(--font-ui); padding: 0.2rem 0.5rem; background: var(--surface-2); border: 1px solid var(--border); border-radius: 4px; color: var(--muted); cursor: pointer; transition: background var(--tr), color var(--tr); }
    .console-header button:hover { background: var(--surface); color: var(--text); }
    .console { width: 100%; max-width: 560px; height: 180px; background: var(--surface); border: 1px solid var(--border); border-radius: var(--radius); padding: 0.65rem 0.85rem; font-family: var(--font-mono); font-size: 0.65rem; line-height: 1.55; color: var(--muted); overflow-y: auto; white-space: pre-wrap; word-break: break-all; }

    /* ── CONFIG PAGE ── */
    #page-config { max-width: 560px; }

    .cfg-page-title {
      font-family: var(--font-mono); font-size: 0.85rem; font-weight: 700; color: var(--accent);
      text-transform: uppercase; letter-spacing: 0.08em;
      width: 100%;
    }

    .cfg-card {
      width: 100%;
      background: var(--surface); border: 1px solid var(--border); border-radius: var(--radius);
      padding: 0.85rem 1rem;
      display: flex; flex-direction: column; gap: 0.65rem;
    }

    .cfg-card-title {
      font-size: 0.65rem; font-weight: 700; text-transform: uppercase; letter-spacing: 0.1em;
      color: var(--accent); border-bottom: 1px solid var(--border); padding-bottom: 0.4rem;
      font-family: var(--font-mono);
    }

    .cfg-grid {
      display: grid;
      grid-template-columns: repeat(2, 1fr);
      gap: 0.55rem;
    }
    .cfg-grid.cols3 { grid-template-columns: repeat(3, 1fr); }
    .cfg-grid.cols4 { grid-template-columns: repeat(4, 1fr); }
    .cfg-grid.cols1 { grid-template-columns: 1fr; }

    .cfg-field { display: flex; flex-direction: column; gap: 0.2rem; }

    .cfg-field label {
      font-size: 0.62rem; font-weight: 600; text-transform: uppercase; letter-spacing: 0.06em; color: var(--muted);
    }

    .cfg-field input[type="text"],
    .cfg-field input[type="password"],
    .cfg-field input[type="number"] {
      padding: 0.28rem 0.45rem;
      background: var(--surface-2); border: 1px solid var(--border); border-radius: 5px;
      color: var(--text); font-family: var(--font-mono); font-size: 0.75rem;
      transition: border-color var(--tr);
      width: 100%;
    }
    .cfg-field input:focus { outline: none; border-color: var(--accent); }
    .cfg-field input::-webkit-outer-spin-button, .cfg-field input::-webkit-inner-spin-button { -webkit-appearance: none; }
    .cfg-field input[type="number"] { appearance: textfield; -moz-appearance: textfield; }
    .cfg-field input:disabled { opacity: 0.5; cursor: not-allowed; }

    .cfg-field .hint { font-size: 0.58rem; color: var(--faint); line-height: 1.3; }

    .cfg-checks { display: flex; flex-wrap: wrap; gap: 0.4rem; }
    .cfg-checks label {
      display: flex; align-items: center; gap: 0.3rem;
      font-size: 0.72rem; color: var(--muted); cursor: pointer;
      padding: 0.25rem 0.55rem;
      background: var(--surface-2); border: 1px solid var(--border); border-radius: 4px;
      transition: border-color var(--tr), color var(--tr);
      font-weight: 500;
    }
    .cfg-checks label:has(input:checked) { border-color: var(--accent); color: var(--accent); }
    .cfg-checks input { display: none; }

    .cfg-actions { display: flex; gap: 0.6rem; width: 100%; }
    .cfg-save-btn {
      flex: 1; padding: 0.65rem;
      background: var(--accent); border: none; border-radius: var(--radius);
      color: #fff; font-size: 0.85rem; font-weight: 700; font-family: var(--font-ui);
      cursor: pointer; transition: opacity var(--tr);
    }
    .cfg-save-btn:hover { opacity: 0.85; }
    .cfg-cancel-btn {
      padding: 0.65rem 1rem;
      background: var(--surface-2); border: 1px solid var(--border); border-radius: var(--radius);
      color: var(--muted); font-size: 0.85rem; font-weight: 600; font-family: var(--font-ui);
      cursor: pointer; transition: background var(--tr), color var(--tr);
    }
    .cfg-cancel-btn:hover { background: var(--surface); color: var(--text); }

    @media (max-width: 420px) {
      .mode-bar { flex-direction: column; align-items: flex-start; }
      .pulse-row { margin-left: 0; }
      .cfg-grid, .cfg-grid.cols3, .cfg-grid.cols4 { grid-template-columns: 1fr; }
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
    <button class="nav-btn" id="nav-main"   onclick="showPage('main')"  >&#9679; Main</button>
    <button class="nav-btn" id="nav-config" onclick="showPage('config')">&#9881; Config</button>
  </div>
</header>

<!-- ══════════════════ MAIN PAGE ══════════════════ -->
<div class="page active" id="page-main">

  <div class="mode-bar" style="max-width:560px;">
    <span class="mode-label">Mode</span>
    <div class="mode-toggle">
      <button id="btn-toggle-mode" class="active" onclick="setMode('toggle')">Toggle</button>
      <button id="btn-pulse-mode"                 onclick="setMode('pulse')">Pulse</button>
    </div>
    <div class="pulse-row" id="pulse-row" style="opacity:0.35; pointer-events:none;">
      <label for="pulse-ms">Duration</label>
      <input type="number" id="pulse-ms" value="500" min="10" max="30000" step="10">
      <span class="unit">ms</span>
    </div>
  </div>

  <div class="grid" id="grid"></div>

  <div class="bottom-row">
    <button class="action-btn"       onclick="allOff()">&#11035; ALL OFF</button>
    <button class="action-btn green" onclick="resetUsed()">&#8635; RESET DEAD</button>
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

</div><!-- /page-main -->

<!-- ══════════════════ CONFIG PAGE ══════════════════ -->
<div class="page" id="page-config">

  <div class="cfg-page-title" style="max-width:560px;">&#9881; Configuration</div>

  <!-- Twitch / Auth -->
  <div class="cfg-card">
    <div class="cfg-card-title">Twitch / Auth</div>
    <div class="cfg-grid">
      <div class="cfg-field">
        <label for="cfg-channel">Channel</label>
        <input type="text" id="cfg-channel" placeholder="channelname">
      </div>
      <div class="cfg-field">
        <label for="cfg-nick">Bot Nickname</label>
        <input type="text" id="cfg-nick" placeholder="bot_username">
      </div>
    </div>
    <div class="cfg-grid cols1">
      <div class="cfg-field">
        <label for="cfg-oauth">OAuth Token</label>
        <input type="password" id="cfg-oauth" placeholder="oauth:xxxxxxxxxxxxxxxxx">
        <span class="hint">Leave blank to keep existing token.</span>
      </div>
    </div>
  </div>

  <!-- Trigger Events -->
  <div class="cfg-card">
    <div class="cfg-card-title">Trigger Events</div>
    <div class="cfg-checks">
      <label><input type="checkbox" id="cb-bits">   Bits</label>
      <label><input type="checkbox" id="cb-points"> Channel Points</label>
      <label><input type="checkbox" id="cb-subs">   Subs</label>
      <label><input type="checkbox" id="cb-raids">  Raids</label>
    </div>
    <div class="cfg-grid">
      <div class="cfg-field">
        <label for="cfg-bits">Bits Threshold (used when Reward ID is blank)</label>
        <input type="number" id="cfg-bits" value="100" min="1" max="1000000" step="1">
      </div>
      <div class="cfg-field">
        <label for="cfg-points-thresh">Points (redemptions)</label>
        <input type="number" id="cfg-points-thresh" value="1" min="1" max="1000" step="1">
      </div>
      <div class="cfg-field">
        <label for="cfg-subs-thresh">Subs Threshold</label>
        <input type="number" id="cfg-subs-thresh" value="1" min="1" max="1000" step="1">
      </div>
      <div class="cfg-field">
        <label for="cfg-raid-thresh">Raid Viewers</label>
        <input type="number" id="cfg-raid-thresh" value="10" min="1" max="100000" step="1">
      </div>
    </div>
    <div class="cfg-grid cols1">
      <div class="cfg-field">
        <label for="cfg-bits-filter">Bits / Power-Up Reward ID</label>
        <input type="text" id="cfg-bits-filter" placeholder="blank = use bits threshold">
        <span class="hint">If set, only this reward ID triggers; bits amount is ignored.</span>
      </div>
    </div>
    <div class="cfg-grid cols1">
      <div class="cfg-field">
        <label for="cfg-pts-filter">Points Reward ID Filter</label>
        <input type="text" id="cfg-pts-filter" placeholder="e.g. a1b2c3d4-e5f6-... (blank = all)">
      </div>
    </div>
  </div>

  <!-- Firing / Timing -->
  <div class="cfg-card">
    <div class="cfg-card-title">Firing &amp; Timing</div>
    <div class="cfg-grid cols3">
      <div class="cfg-field">
        <label for="cfg-pulse">Pulse Duration (ms)</label>
        <input type="number" id="cfg-pulse" value="500" min="10" max="30000" step="10">
      </div>
      <div class="cfg-field">
        <label for="cfg-min-gap">Min Gap Between Fires (ms)</label>
        <input type="number" id="cfg-min-gap" value="2000" min="0" max="60000" step="100">
      </div>
      <div class="cfg-field">
        <label for="cfg-cs-delay">CS Sense Delay (ms)</label>
        <input type="number" id="cfg-cs-delay" value="10" min="1" max="500" step="1">
        <span class="hint">Wait after fire before ADC read.</span>
      </div>
    </div>
  </div>

  <!-- Current Sensor Calibration (read-only — firmware defines) -->
  <div class="cfg-card">
    <div class="cfg-card-title">Current Sensor — firmware #defines (read-only)</div>
    <div class="cfg-grid cols3">
      <div class="cfg-field">
        <label>CS Midpoint (V)</label>
        <input type="number" id="info-cs-mid" disabled placeholder="2.5">
        <span class="hint">CS_MIDPOINT_V</span>
      </div>
      <div class="cfg-field">
        <label>Sensitivity (mV/A)</label>
        <input type="number" id="info-cs-mva" disabled placeholder="-100">
        <span class="hint">CS_MV_PER_AMP</span>
      </div>
      <div class="cfg-field">
        <label>Detect Threshold (A)</label>
        <input type="number" id="info-cs-det" disabled placeholder="3.0">
        <span class="hint">CS_DETECT_AMPS</span>
      </div>
    </div>
    <div class="cfg-grid">
      <div class="cfg-field">
        <label>ADC Moving Avg Samples</label>
        <input type="number" id="info-adc-ma" disabled placeholder="10">
        <span class="hint">ADC_MA_SAMPLES</span>
      </div>
      <div class="cfg-field">
        <label>Current Sensor Pin</label>
        <input type="text" id="info-pin-cur" disabled placeholder="GPIO 0">
        <span class="hint">PIN_CURRENT</span>
      </div>
    </div>
  </div>

  <!-- Hardware Pins (read-only — firmware defines) -->
  <div class="cfg-card">
    <div class="cfg-card-title">Shift Register Pins — firmware #defines (read-only)</div>
    <div class="cfg-grid cols4">
      <div class="cfg-field">
        <label>DATA (DS/SER)</label>
        <input type="text" id="info-pin-data" disabled placeholder="GPIO 1">
      </div>
      <div class="cfg-field">
        <label>CLOCK (SRCLK)</label>
        <input type="text" id="info-pin-clk" disabled placeholder="GPIO 4">
      </div>
      <div class="cfg-field">
        <label>LATCH (RCLK)</label>
        <input type="text" id="info-pin-lat" disabled placeholder="GPIO 2">
      </div>
      <div class="cfg-field">
        <label>OE (active-low)</label>
        <input type="text" id="info-pin-oe" disabled placeholder="GPIO 5">
      </div>
    </div>
  </div>

  <!-- Network (read-only) -->
  <div class="cfg-card">
    <div class="cfg-card-title">Network — firmware #defines (read-only)</div>
    <div class="cfg-grid">
      <div class="cfg-field">
        <label>Hostname</label>
        <input type="text" id="info-hostname" disabled placeholder="twitchblows">
        <span class="hint">HOSTNAME — mDNS &amp; AP SSID</span>
      </div>
      <div class="cfg-field">
        <label>WiFi STA Timeout (ms)</label>
        <input type="text" id="info-wifi-to" disabled placeholder="5000">
        <span class="hint">WIFI_TIMEOUT</span>
      </div>
    </div>
  </div>

  <!-- Debug (read-only) -->
  <div class="cfg-card">
    <div class="cfg-card-title">Debug — firmware #defines (read-only)</div>
    <div class="cfg-grid cols3">
      <div class="cfg-field">
        <label>Serial Debug (DEBUG)</label>
        <input type="text" id="info-debug" disabled placeholder="1">
        <span class="hint">0 = silent, 1 = verbose</span>
      </div>
    </div>
  </div>

  <div class="cfg-actions">
    <button class="cfg-save-btn"   onclick="saveCfg()">&#10003; Save Config</button>
    <button class="cfg-cancel-btn" onclick="showPage('main')">Cancel</button>
  </div>

</div><!-- /page-config -->

<script>
  // ── State ──────────────────────────────────
  let mode        = 'toggle';
  let activeQ     = -1;
  let pulseTimers = {};
  let pulseBars   = {};
  let usedMask    = 0;
  let manualMask  = 0;
  let twitchConn  = false;
  let nextQ       = 0;

  // ── Page navigation ────────────────────────
  function showPage(name) {
    document.querySelectorAll('.page').forEach(p => p.classList.remove('active'));
    document.querySelectorAll('.nav-btn').forEach(b => b.classList.remove('active'));
    document.getElementById('page-' + name).classList.add('active');
    document.getElementById('nav-' + name).classList.add('active');
    if (name === 'config') loadCfg();
  }
  document.getElementById('nav-main').classList.add('active');

  // ── Build output buttons ──────────────────
  const grid = document.getElementById('grid');
  for (let i = 0; i < 16; i++) {
    const btn = document.createElement('button');
    btn.className = 'output-btn';
    btn.id = 'btn' + i;
    btn.setAttribute('aria-label', 'Channel ' + (i+1));
    btn.innerHTML =
      '<span class="q-label">Ch' + (i+1) + '</span>' +
      '<span class="q-sub" id="sub' + i + '">\u2014</span>' +
      '<button class="q-toggle" id="tog' + i + '" onclick="event.stopPropagation(); toggleChan(' + i + ')" title="Mark channel dead/live">\u2713</button>';
    btn.onclick = () => handleBtnClick(i);
    grid.appendChild(btn);
  }

  // ── Persistence helpers ────────────────────
  function savePrefs() {
    try {
      localStorage.setItem('tb_mode',    mode);
      localStorage.setItem('tb_pulsems', document.getElementById('pulse-ms').value);
    } catch(e) {}
  }
  function loadPrefs() {
    try {
      const savedMode = localStorage.getItem('tb_mode');
      const savedMs   = localStorage.getItem('tb_pulsems');
      if (savedMode === 'toggle' || savedMode === 'pulse') mode = savedMode;
      if (savedMs !== null) {
        const ms = parseInt(savedMs);
        if (ms >= 10 && ms <= 30000) document.getElementById('pulse-ms').value = ms;
      }
    } catch(e) {}
  }
  loadPrefs();
  applyModeUI(mode);
  document.getElementById('pulse-ms').addEventListener('change', savePrefs);

  // ── Mode ───────────────────────────────────
  function setMode(m) { mode = m; applyModeUI(m); savePrefs(); }
  function applyModeUI(m) {
    document.getElementById('btn-toggle-mode').classList.toggle('active', m === 'toggle');
    document.getElementById('btn-pulse-mode' ).classList.toggle('active', m === 'pulse');
    const pr = document.getElementById('pulse-row');
    pr.style.opacity      = m === 'pulse' ? '1'    : '0.35';
    pr.style.pointerEvents = m === 'pulse' ? 'auto' : 'none';
  }

  // ── Output click ───────────────────────────
  function handleBtnClick(i) {
    const btn = document.getElementById('btn' + i);
    if (btn.classList.contains('dead')) return;
    if (mode === 'toggle') {
      const turning_on = (activeQ !== i);
      fetch('/toggle?q=' + i).catch(() => {});
      if (activeQ !== -1) document.getElementById('btn' + activeQ).classList.remove('active-toggle');
      activeQ = turning_on ? i : -1;
      if (turning_on) btn.classList.add('active-toggle');
    } else {
      const ms = parseInt(document.getElementById('pulse-ms').value) || 500;
      fetch('/pulse?q=' + i + '&ms=' + ms).catch(() => {});
      startPulseUI(i, ms);
    }
  }

  function startPulseUI(i, ms) {
    const btn = document.getElementById('btn' + i);
    if (pulseTimers[i]) clearTimeout(pulseTimers[i]);
    let bar = btn.querySelector('.pulse-bar');
    if (!bar) { bar = document.createElement('div'); bar.className = 'pulse-bar'; btn.appendChild(bar); }
    btn.classList.add('active-pulse');
    const start = Date.now();
    if (pulseBars[i]) cancelAnimationFrame(pulseBars[i]);
    function animBar() {
      const frac = Math.max(0, 1 - (Date.now() - start) / ms);
      bar.style.transform = 'scaleX(' + frac + ')';
      if (frac > 0) pulseBars[i] = requestAnimationFrame(animBar);
    }
    animBar();
    pulseTimers[i] = setTimeout(() => {
      btn.classList.remove('active-pulse');
      bar.remove();
      delete pulseTimers[i];
      delete pulseBars[i];
    }, ms + 80);
  }

  function toggleChan(i) {
    const isUsed = (usedMask >> i) & 1;
    fetch('/togglechan?q=' + i + '&used=' + (isUsed ? '0' : '1')).catch(() => {});
    if (isUsed) usedMask &= ~(1 << i);
    else        usedMask |=  (1 << i);
    updateGrid();
  }

  function allOff() {
    fetch('/alloff').catch(() => {});
    if (activeQ !== -1) { document.getElementById('btn' + activeQ).classList.remove('active-toggle'); activeQ = -1; }
  }

  function resetUsed() {
    fetch('/resetused').catch(() => {});
    usedMask = 0;
    updateGrid();
  }

  // ── Grid update ────────────────────────────
  function updateGrid() {
    for (let i = 0; i < 16; i++) {
      const btn = document.getElementById('btn' + i);
      const dead = ((usedMask >> i) & 1) || ((manualMask >> i) & 1);
      btn.classList.toggle('dead', dead);
      btn.classList.toggle('next', !dead && i === nextQ);
    }
  }

  // ── State polling ──────────────────────────
  function pollState() {
    fetch('/status')
      .then(r => r.json())
      .then(data => {
        usedMask   = data.usedMask   || 0;
        manualMask = data.manualMask || 0;
        activeQ    = data.activeQ    !== undefined ? data.activeQ : -1;
        twitchConn = data.twitchConn || false;
        nextQ      = data.nextQ      !== undefined ? data.nextQ : 0;

        updateGrid();

        const dot = document.getElementById('twitch-dot');
        dot.classList.toggle('connected', twitchConn);

        const sdot = document.getElementById('status-dot');
        sdot.className = 'status-dot ' + (data.sensorReady ? 'ok' : 'warn');
        document.getElementById('status-text').textContent =
          data.sensorReady ? 'Sensor OK' : 'Sensor fault \u2014 outputs disabled';

        if (data.adcCurr !== undefined) {
          document.getElementById('adc-curr').textContent = data.adcCurr + ' mV';
          document.getElementById('adc-max' ).textContent = data.adcMax  + ' mV';
          document.getElementById('adc-min' ).textContent = data.adcMin  + ' mV';
        }
        if (data.ampCurr !== undefined) {
          document.getElementById('amp-curr').textContent = data.ampCurr.toFixed(2) + ' A';
          document.getElementById('amp-max' ).textContent = data.ampMax.toFixed(2)  + ' A';
          document.getElementById('amp-min' ).textContent = data.ampMin.toFixed(2)  + ' A';
        }

        for (let i = 0; i < 16; i++) document.getElementById('btn' + i).classList.remove('active-toggle');
        if (activeQ >= 0 && activeQ < 16 && mode === 'toggle')
          document.getElementById('btn' + activeQ).classList.add('active-toggle');

        if (data.chanPeaks) {
          for (let i = 0; i < 16; i++) {
            const sub = document.getElementById('sub' + i);
            sub.textContent = data.chanPeaks[i] ? (data.chanPeaks[i] + ' mA') : '\u2014';
          }
        }
      })
      .catch(() => {});
  }

  // ── Config: load ──────────────────────────
  function loadCfg() {
    fetch('/getcfg')
      .then(r => r.json())
      .then(d => {
        document.getElementById('cfg-channel'      ).value   = d.channel         || '';
        document.getElementById('cfg-bits'         ).value   = d.bitsThreshold   || 100;
        document.getElementById('cfg-points-thresh').value   = d.pointsThreshold || 1;
        document.getElementById('cfg-subs-thresh'  ).value   = d.subsThreshold   || 1;
        document.getElementById('cfg-raid-thresh'  ).value   = d.raidThreshold   || 10;
        document.getElementById('cfg-pulse'        ).value   = d.pulseDurMs      || 500;
        document.getElementById('cfg-min-gap'      ).value   = d.minGapMs        !== undefined ? d.minGapMs : 2000;
        document.getElementById('cfg-cs-delay'     ).value   = d.csDelayMs       || 10;
        document.getElementById('cfg-pts-filter'   ).value   = d.ptsFilter       || '';
        document.getElementById('cfg-bits-filter'  ).value   = d.bitsFilter      || '';
        document.getElementById('cb-bits'   ).checked = d.evBits   === true || d.evBits   === 'true';
        document.getElementById('cb-points' ).checked = d.evPoints === true || d.evPoints === 'true';
        document.getElementById('cb-subs'   ).checked = d.evSubs   === true || d.evSubs   === 'true';
        document.getElementById('cb-raids'  ).checked = d.evRaids  === true || d.evRaids  === 'true';
        // Read-only firmware info (if firmware sends them)
        if (d.csMidV   !== undefined) document.getElementById('info-cs-mid' ).placeholder = d.csMidV;
        if (d.csMvA    !== undefined) document.getElementById('info-cs-mva' ).placeholder = d.csMvA;
        if (d.csDetA   !== undefined) document.getElementById('info-cs-det' ).placeholder = d.csDetA;
        if (d.adcMaSmp !== undefined) document.getElementById('info-adc-ma' ).placeholder = d.adcMaSmp;
        if (d.pinCur   !== undefined) document.getElementById('info-pin-cur').placeholder = 'GPIO ' + d.pinCur;
        if (d.pinData  !== undefined) document.getElementById('info-pin-data').placeholder = 'GPIO ' + d.pinData;
        if (d.pinClk   !== undefined) document.getElementById('info-pin-clk').placeholder  = 'GPIO ' + d.pinClk;
        if (d.pinLat   !== undefined) document.getElementById('info-pin-lat').placeholder  = 'GPIO ' + d.pinLat;
        if (d.pinOE    !== undefined) document.getElementById('info-pin-oe').placeholder   = 'GPIO ' + d.pinOE;
        if (d.hostname !== undefined) document.getElementById('info-hostname').placeholder  = d.hostname;
        if (d.wifiTo   !== undefined) document.getElementById('info-wifi-to').placeholder  = d.wifiTo;
        if (d.debug    !== undefined) document.getElementById('info-debug').placeholder     = d.debug;
        document.getElementById('cfg-oauth').value = '';
        document.getElementById('cfg-nick' ).value = '';
      })
      .catch(() => {});
  }

  // ── Config: save ──────────────────────────
  function saveCfg() {
    const params = new URLSearchParams();
    const ch         = document.getElementById('cfg-channel'      ).value.trim();
    const bits       = document.getElementById('cfg-bits'         ).value;
    const ptThresh   = document.getElementById('cfg-points-thresh').value;
    const subThresh  = document.getElementById('cfg-subs-thresh'  ).value;
    const raidThresh = document.getElementById('cfg-raid-thresh'  ).value;
    const pulse      = document.getElementById('cfg-pulse'        ).value;
    const minGap     = document.getElementById('cfg-min-gap'      ).value;
    const csDelay    = document.getElementById('cfg-cs-delay'     ).value;
    const ptsFilter  = document.getElementById('cfg-pts-filter'   ).value.trim();
    const bitsFilter = document.getElementById('cfg-bits-filter'  ).value.trim();
    const oauth      = document.getElementById('cfg-oauth'        ).value.trim();
    const nick       = document.getElementById('cfg-nick'         ).value.trim();

    if (ch)        params.append('channel',          ch);
    if (bits)      params.append('bits_threshold',   bits);
    if (ptThresh)  params.append('points_threshold', ptThresh);
    if (subThresh) params.append('subs_threshold',   subThresh);
    if (raidThresh) params.append('raid_threshold',  raidThresh);
    if (pulse)     params.append('pulse_ms',         pulse);
    if (minGap !== '') params.append('min_gap_ms',   minGap);
    if (csDelay)   params.append('cs_delay_ms',      csDelay);
    params.append('pts_filter', ptsFilter);
    params.append('bits_filter', bitsFilter);
    params.append('ev_bits',   document.getElementById('cb-bits'  ).checked ? '1' : '0');
    params.append('ev_points', document.getElementById('cb-points').checked ? '1' : '0');
    params.append('ev_subs',   document.getElementById('cb-subs'  ).checked ? '1' : '0');
    params.append('ev_raids',  document.getElementById('cb-raids' ).checked ? '1' : '0');
    if (oauth) params.append('oauth', oauth);
    if (nick)  params.append('nick',  nick);

    fetch('/savecfg', { method: 'POST', body: params })
      .then(r => r.json())
      .then(data => {
        if (data.ok) {
          document.getElementById('status-text').textContent = 'Config saved';
          showPage('main');
        }
      })
      .catch(() => { document.getElementById('status-text').textContent = 'Error saving config'; });
  }

  // ── Console polling ────────────────────────
  let consoleSeen  = '';
  let consoleLines = [];
  function pollLog() {
    fetch('/log')
      .then(r => r.text())
      .then(txt => {
        if (txt !== consoleSeen) {
          consoleSeen  = txt;
          consoleLines = txt.split('\n').filter(l => l.length > 0);
          if (consoleLines.length > 500) consoleLines = consoleLines.slice(-500);
          const el = document.getElementById('console');
          const atBottom = el.scrollHeight - el.scrollTop - el.clientHeight < 40;
          el.textContent = consoleLines.join('\n');
          if (atBottom) el.scrollTop = el.scrollHeight;
        }
      })
      .catch(() => {});
  }
  function clearConsole() {
    document.getElementById('console').textContent = '';
    consoleSeen  = '';
    consoleLines = [];
  }

  // ── Init ──────────────────────────────────
  pollState();
  pollLog();
  setInterval(pollState, 3000);
  setInterval(pollLog,   1000);
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

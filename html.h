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
      --green-dim: #22c55e22;
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

    .mode-toggle button.active {
      background: var(--accent);
      color: #fff;
    }

    /* ── Pulse ms row ── */
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

    .pulse-row input[type="number"]:focus {
      outline: none;
      border-color: var(--accent);
    }

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

    .output-btn:active { transform: scale(0.96); }
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
    .output-btn.active-toggle::before {
      background: var(--accent);
      box-shadow: 0 0 6px var(--accent);
    }

    .output-btn.active-pulse {
      background: var(--amber-dim);
      border-color: var(--amber);
      color: var(--amber);
      box-shadow: 0 0 16px var(--amber-dim);
      animation: pulse-glow 0.7s ease-in-out infinite alternate;
    }
    .output-btn.active-pulse::before {
      background: var(--amber);
      box-shadow: 0 0 6px var(--amber);
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

    .all-off-btn:hover {
      background: var(--surface-2);
      border-color: var(--accent);
      color: var(--accent);
    }

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

    #status-text {
      font-size: 0.8rem;
      color: var(--muted);
      font-family: var(--font-mono);
    }

    @media (max-width: 420px) {
      .mode-bar { flex-direction: column; align-items: flex-start; }
      .pulse-row { margin-left: 0; }
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
</div>

<div class="status-bar">
  <div class="status-dot" id="status-dot"></div>
  <span id="status-text">Loading&#8230;</span>
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

  // ── Build buttons ─────────────────────────
  const grid = document.getElementById('grid');
  for (let i = 0; i < 8; i++) {
    const btn = document.createElement('button');
    btn.className = 'output-btn';
    btn.id = 'btn' + i;
    btn.setAttribute('aria-label', 'Output Q' + i);
    btn.innerHTML =
      '<span class="q-label">Q' + i + '</span>' +
      '<span class="q-sub" id="sub' + i + '">&#8212;</span>';
    btn.onclick = () => handleBtnClick(i);
    grid.appendChild(btn);
  }

  // ── Restore prefs before rendering ────────
  loadPrefs();
  applyModeUI(mode);

  // Save duration whenever it changes
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

  // ── All off ───────────────────────────────
  function allOff() {
    for (let i = 0; i < 8; i++) cancelPulse(i);
    sendSet(-1);
  }

  // ── UI updates ────────────────────────────
  function updateToggleUI() {
    for (let i = 0; i < 8; i++) {
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

  // ── Init ──────────────────────────────────
  fetch('/state')
    .then(r => r.json())
    .then(data => { activeQ = data.active; updateToggleUI(); updateStatus(); })
    .catch(() => setStatusText('Could not reach device'));
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

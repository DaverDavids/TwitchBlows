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
      --radius:    8px;
      --font-mono: 'JetBrains Mono', monospace;
      --font-ui:   'Inter', sans-serif;
      --tr: 180ms cubic-bezier(0.16,1,0.3,1);
    }
    html,body { min-height:100dvh; background:var(--bg); color:var(--text); font-family:var(--font-ui); font-size:15px; -webkit-font-smoothing:antialiased; }
    body { display:flex; flex-direction:column; align-items:center; padding:1.25rem 0.75rem 2.5rem; gap:1rem; }

    header { display:flex; align-items:center; gap:0.6rem; width:100%; max-width:580px; }
    .logo-mark { width:30px; height:30px; flex-shrink:0; }
    h1 { font-family:var(--font-mono); font-size:1.2rem; font-weight:700; letter-spacing:-0.02em; color:var(--accent); }
    h1 span { color:var(--text); }
    .header-right { margin-left:auto; display:flex; align-items:center; gap:0.6rem; }
    .twitch-dot { display:flex; align-items:center; gap:0.3rem; font-size:0.7rem; font-weight:600; color:var(--muted); font-family:var(--font-mono); }
    .twitch-dot .dot { width:7px; height:7px; border-radius:50%; background:var(--faint); transition:background var(--tr),box-shadow var(--tr); }
    .twitch-dot.connected .dot { background:var(--green); box-shadow:0 0 5px var(--green); }
    .twitch-dot.connected { color:var(--green); }
    .cfg-link { padding:0.3rem 0.7rem; background:var(--surface-2); border:1px solid var(--border); border-radius:5px; color:var(--muted); font-size:0.75rem; font-weight:600; font-family:var(--font-ui); cursor:pointer; text-decoration:none; transition:background var(--tr),color var(--tr),border-color var(--tr); }
    .cfg-link:hover { border-color:var(--accent); color:var(--accent); }

    .card { width:100%; max-width:580px; background:var(--surface); border:1px solid var(--border); border-radius:var(--radius); padding:0.75rem 0.9rem; }
    .card-title { font-size:0.68rem; font-weight:700; text-transform:uppercase; letter-spacing:0.08em; color:var(--muted); margin-bottom:0.55rem; }

    .mode-bar { display:flex; align-items:center; gap:0.75rem; flex-wrap:wrap; }
    .mode-toggle { display:flex; background:var(--surface-2); border:1px solid var(--border); border-radius:5px; overflow:hidden; }
    .mode-toggle button { padding:0.28rem 0.7rem; font-size:0.75rem; font-weight:600; font-family:var(--font-ui); background:none; border:none; color:var(--muted); cursor:pointer; transition:background var(--tr),color var(--tr); white-space:nowrap; }
    .mode-toggle button.active { background:var(--accent); color:#fff; }
    .pulse-row { display:flex; align-items:center; gap:0.4rem; margin-left:auto; }
    .pulse-row label { font-size:0.72rem; color:var(--muted); font-weight:500; white-space:nowrap; }
    .pulse-row input[type="number"] { width:72px; padding:0.28rem 0.4rem; background:var(--surface-2); border:1px solid var(--border); border-radius:5px; color:var(--text); font-family:var(--font-mono); font-size:0.78rem; font-weight:600; text-align:center; appearance:textfield; -moz-appearance:textfield; transition:border-color var(--tr); }
    .pulse-row input[type="number"]:focus { outline:none; border-color:var(--accent); }
    .pulse-row input::-webkit-outer-spin-button, .pulse-row input::-webkit-inner-spin-button { -webkit-appearance:none; }
    .pulse-row .unit { font-size:0.7rem; color:var(--muted); }

    .grid { display:grid; grid-template-columns:repeat(4,1fr); gap:0.55rem; width:100%; max-width:580px; }
    .output-btn { position:relative; display:flex; flex-direction:column; align-items:center; justify-content:center; gap:0.2rem; padding:0.9rem 0.4rem 0.7rem; background:var(--surface); border:1.5px solid var(--border); border-radius:var(--radius); color:var(--muted); cursor:pointer; font-family:var(--font-mono); font-weight:700; transition:background var(--tr),border-color var(--tr),color var(--tr),box-shadow var(--tr),transform 80ms ease; user-select:none; -webkit-tap-highlight-color:transparent; }
    .output-btn:active:not(.dead) { transform:scale(0.95); }
    .output-btn .q-label { font-size:0.92rem; }
    .output-btn .q-sub { font-size:0.7rem; font-family:var(--font-ui); font-weight:600; opacity:0.85; }
    .output-btn::before { content:''; position:absolute; top:6px; right:8px; width:6px; height:6px; border-radius:50%; background:var(--faint); transition:background var(--tr),box-shadow var(--tr); }
    .output-btn.active-toggle { background:var(--accent-dim); border-color:var(--accent); color:var(--accent); box-shadow:0 0 14px var(--accent-dim); }
    .output-btn.active-toggle::before { background:var(--accent); box-shadow:0 0 5px var(--accent); }
    .output-btn.active-pulse { background:var(--amber-dim); border-color:var(--amber); color:var(--amber); box-shadow:0 0 14px var(--amber-dim); animation:pulse-glow 0.7s ease-in-out infinite alternate; }
    .output-btn.active-pulse::before { background:var(--amber); box-shadow:0 0 5px var(--amber); }
    .output-btn.dead { background:var(--surface); border-color:var(--faint); color:var(--faint); cursor:not-allowed; opacity:0.55; }
    .output-btn.dead::before { background:var(--red); opacity:0.5; }
    .output-btn.next::after { content:'\\25B6'; position:absolute; top:5px; left:6px; font-size:0.55rem; color:var(--green); font-weight:700; }
    .output-btn .q-toggle { position:absolute; bottom:3px; right:5px; font-size:0.5rem; cursor:pointer; opacity:0.35; transition:opacity var(--tr); background:none; border:none; color:var(--muted); padding:2px 3px; line-height:1; }
    .output-btn .q-toggle:hover { opacity:1; color:var(--accent); }
    @keyframes pulse-glow { from{box-shadow:0 0 6px var(--amber-dim);}  to{box-shadow:0 0 18px var(--amber);} }
    .pulse-bar { position:absolute; bottom:0; left:0; height:2px; background:var(--amber); border-radius:0 0 var(--radius) var(--radius); width:100%; transform-origin:left; transform:scaleX(1); }

    .bottom-row { display:flex; gap:0.55rem; width:100%; max-width:580px; }
    .action-btn { flex:1; padding:0.65rem; background:var(--surface); border:1.5px solid var(--border); border-radius:var(--radius); color:var(--muted); font-size:0.78rem; font-weight:600; font-family:var(--font-ui); cursor:pointer; transition:background var(--tr),border-color var(--tr),color var(--tr); }
    .action-btn.off:hover  { border-color:var(--accent); color:var(--accent); }
    .action-btn.rst:hover  { border-color:var(--green);  color:var(--green); }

    .status-bar { display:flex; align-items:center; gap:0.5rem; }
    .status-dot { width:7px; height:7px; border-radius:50%; background:var(--faint); flex-shrink:0; transition:background var(--tr); }
    .status-dot.on-toggle { background:var(--accent); box-shadow:0 0 5px var(--accent); }
    .status-dot.on-pulse  { background:var(--amber);  box-shadow:0 0 5px var(--amber); }
    #status-text { font-size:0.73rem; color:var(--muted); font-family:var(--font-mono); }

    .sensor-row { display:flex; gap:1.2rem; flex-wrap:wrap; }
    .sensor-group { display:flex; flex-direction:column; gap:0.18rem; }
    .sensor-group .sg-label { font-size:0.62rem; font-weight:700; text-transform:uppercase; letter-spacing:0.07em; color:var(--muted); }
    .sensor-vals { display:flex; gap:0.6rem; align-items:baseline; }
    .sv-pair { display:flex; flex-direction:column; align-items:center; gap:0.05rem; }
    .sv-tag  { font-size:0.58rem; color:var(--muted); text-transform:uppercase; }
    .sv-val  { font-size:0.82rem; font-family:var(--font-mono); font-weight:700; color:var(--green); }

    .console-header { display:flex; align-items:center; justify-content:space-between; width:100%; max-width:580px; font-size:0.7rem; color:var(--muted); font-weight:600; text-transform:uppercase; letter-spacing:0.07em; }
    .console-header button { font-size:0.65rem; padding:2px 7px; background:var(--surface-2); border:1px solid var(--border); border-radius:4px; color:var(--muted); cursor:pointer; }
    .console-header button:hover { background:var(--surface); color:var(--text); }
    .console { width:100%; max-width:580px; background:#0a0c10; border:1px solid var(--border); border-radius:var(--radius); padding:0.6rem; font-family:var(--font-mono); font-size:0.68rem; color:#7ee787; height:170px; overflow-y:auto; white-space:pre-wrap; word-break:break-all; }

    @media (max-width:420px) { .mode-bar{flex-direction:column;align-items:flex-start;} .pulse-row{margin-left:0;} }
  </style>
</head>
<body>

<header>
  <svg class="logo-mark" viewBox="0 0 36 36" fill="none" aria-label="TwitchBlows" xmlns="http://www.w3.org/2000/svg">
    <rect width="36" height="36" rx="8" fill="#e94560" fill-opacity="0.12"/>
    <path d="M9 12 L18 7 L27 12 L27 24 L18 29 L9 24 Z" stroke="#e94560" stroke-width="1.8" fill="none"/>
    <circle cx="18" cy="18" r="4" fill="#e94560"/>
    <path d="M18 14 L18 7 M18 22 L18 29 M14 16 L9 12 M22 20 L27 24 M14 20 L9 24 M22 16 L27 12" stroke="#e94560" stroke-width="1.4" opacity="0.5"/>
  </svg>
  <h1><span>Twitch</span>Blows</h1>
  <div class="header-right">
    <div class="twitch-dot" id="twitch-dot"><div class="dot"></div><span>Twitch</span></div>
    <a class="cfg-link" href="/config">&#9881; Config</a>
  </div>
</header>

<div class="card">
  <div class="card-title">Mode</div>
  <div class="mode-bar">
    <div class="mode-toggle">
      <button id="btn-toggle-mode" class="active" onclick="setMode('toggle')">Toggle</button>
      <button id="btn-pulse-mode" onclick="setMode('pulse')">Pulse</button>
    </div>
    <div class="pulse-row" id="pulse-row" style="opacity:0.35;pointer-events:none;">
      <label for="pulse-ms">Duration</label>
      <input type="number" id="pulse-ms" value="500" min="10" max="30000" step="10">
      <span class="unit">ms</span>
    </div>
  </div>
</div>

<div class="grid" id="grid"></div>

<div class="bottom-row">
  <button class="action-btn off" onclick="allOff()">&#11035; ALL OFF</button>
  <button class="action-btn rst" onclick="resetUsed()">&#8635; RESET DEAD</button>
</div>

<div class="card">
  <div class="card-title">Current Sensor</div>
  <div class="sensor-row">
    <div class="sensor-group">
      <span class="sg-label">ADC (mV)</span>
      <div class="sensor-vals">
        <div class="sv-pair"><span class="sv-tag">Now</span><span class="sv-val" id="adc-curr">--</span></div>
        <div class="sv-pair"><span class="sv-tag">Max</span><span class="sv-val" id="adc-max">--</span></div>
        <div class="sv-pair"><span class="sv-tag">Min</span><span class="sv-val" id="adc-min">--</span></div>
      </div>
    </div>
    <div class="sensor-group">
      <span class="sg-label">Current (A)</span>
      <div class="sensor-vals">
        <div class="sv-pair"><span class="sv-tag">Now</span><span class="sv-val" id="amp-curr">--</span></div>
        <div class="sv-pair"><span class="sv-tag">Max</span><span class="sv-val" id="amp-max">--</span></div>
        <div class="sv-pair"><span class="sv-tag">Min</span><span class="sv-val" id="amp-min">--</span></div>
      </div>
    </div>
  </div>
</div>

<div class="card status-bar">
  <div class="status-dot" id="status-dot"></div>
  <span id="status-text">Loading&#8230;</span>
</div>

<div class="console-header">
  <span>&#9679; Device Console</span>
  <button onclick="clearConsole()">Clear</button>
</div>
<div class="console" id="console">Connecting...</div>

<script>
  const LS_MODE='tb_mode', LS_MS='tb_pulsems';
  function savePrefs(){try{localStorage.setItem(LS_MODE,mode);localStorage.setItem(LS_MS,document.getElementById('pulse-ms').value);}catch(e){}}
  function loadPrefs(){try{const m=localStorage.getItem(LS_MODE),ms=localStorage.getItem(LS_MS);if(m==='toggle'||m==='pulse')mode=m;if(ms!==null){const v=parseInt(ms);if(v>=10&&v<=30000)document.getElementById('pulse-ms').value=v;}}catch(e){}}

  let mode='toggle', activeQ=-1, pulseTimers={}, pulseBars={}, usedMask=0, twitchConn=false, nextQ=0;

  const grid=document.getElementById('grid');
  for(let i=0;i<16;i++){
    const btn=document.createElement('button');
    btn.className='output-btn'; btn.id='btn'+i;
    btn.setAttribute('aria-label','Channel '+(i+1));
    btn.innerHTML='<span class="q-label">Ch'+(i+1)+'</span><span class="q-sub" id="sub'+i+'">&#8212;</span><button class="q-toggle" id="tog'+i+'" onclick="event.stopPropagation();toggleChan('+i+')" title="Mark dead/live">\u2713</button>';
    btn.onclick=()=>handleBtnClick(i);
    grid.appendChild(btn);
  }

  loadPrefs(); applyModeUI(mode);
  document.getElementById('pulse-ms').addEventListener('change',savePrefs);
  document.getElementById('pulse-ms').addEventListener('input',savePrefs);

  function setMode(m){mode=m;applyModeUI(m);savePrefs();}
  function applyModeUI(m){
    document.getElementById('btn-toggle-mode').classList.toggle('active',m==='toggle');
    document.getElementById('btn-pulse-mode').classList.toggle('active',m==='pulse');
    const pr=document.getElementById('pulse-row');
    pr.style.opacity=m==='pulse'?'1':'0.35'; pr.style.pointerEvents=m==='pulse'?'auto':'none';
  }

  function handleBtnClick(i){
    const btn=document.getElementById('btn'+i);
    if(btn&&btn.classList.contains('dead'))return;
    if(mode==='toggle'){sendSet(activeQ===i?-1:i);}
    else{const ms=Math.max(10,parseInt(document.getElementById('pulse-ms').value)||500);sendPulse(i,ms);}
  }

  function sendSet(q){fetch('/set?q='+q).then(r=>r.json()).then(data=>{activeQ=data.active;updateToggleUI();updateStatus();}).catch(()=>setStatusText('Error'));}
  function sendPulse(q,ms){cancelPulse(q);fetch('/pulse?q='+q+'&ms='+ms).then(r=>r.json()).then(data=>{if(data.ok)startPulseUI(q,ms);}).catch(()=>setStatusText('Error'));}
  function cancelPulse(q){
    if(pulseTimers[q]){clearTimeout(pulseTimers[q]);delete pulseTimers[q];}
    if(pulseBars[q]){cancelAnimationFrame(pulseBars[q].raf);delete pulseBars[q];}
    const btn=document.getElementById('btn'+q);
    if(btn){btn.classList.remove('active-pulse');const bar=btn.querySelector('.pulse-bar');if(bar)bar.remove();document.getElementById('sub'+q).textContent=btn.dataset.peak||'\u2014';}
  }
  function startPulseUI(q,ms){
    const btn=document.getElementById('btn'+q);
    btn.classList.add('active-pulse');
    let bar=btn.querySelector('.pulse-bar');
    if(!bar){bar=document.createElement('div');bar.className='pulse-bar';btn.appendChild(bar);}
    const start=performance.now();
    function tick(now){const e=now-start,p=Math.min(e/ms,1);bar.style.transform='scaleX('+(1-p)+')';document.getElementById('sub'+q).textContent=Math.max(0,(ms-e)/1000).toFixed(1)+'s';if(p<1)pulseBars[q]={raf:requestAnimationFrame(tick)};}
    pulseBars[q]={raf:requestAnimationFrame(tick)};
    document.getElementById('status-dot').className='status-dot on-pulse';
    document.getElementById('status-text').textContent='Q'+q+' pulsing '+ms+'ms';
    pulseTimers[q]=setTimeout(()=>{cancelPulse(q);updateStatus();},ms+300);
  }

  function toggleChan(q){
    const isDead=(usedMask&(1<<q))!==0;
    fetch('/setdead?q='+q+'&dead='+(isDead?'0':'1')).then(r=>r.json()).then(data=>{if(data.ok){if(!isDead)usedMask|=(1<<q);else usedMask&=~(1<<q);updateDeadUI();}}).catch(()=>{});
  }

  function allOff(){
    for(let i=0;i<16;i++)cancelPulse(i);sendSet(-1);
    fetch('/disableall').then(r=>r.json()).then(data=>{if(data.ok){usedMask=0xFFFF;updateDeadUI();setStatusText('All channels disabled');}}).catch(()=>{});
  }
  function resetUsed(){
    fetch('/resetused').then(r=>r.json()).then(data=>{if(data.ok){usedMask=0;updateDeadUI();setStatusText('Dead outputs reset');}}).catch(()=>{});
  }

  function updateDeadUI(){for(let i=0;i<16;i++){const btn=document.getElementById('btn'+i),tog=document.getElementById('tog'+i),dead=(usedMask&(1<<i))!==0;if(btn)btn.classList.toggle('dead',dead);if(tog)tog.textContent=dead?'\u2717':'\u2713';}}
  function updateNextUI(){for(let i=0;i<16;i++){const btn=document.getElementById('btn'+i);if(btn)btn.classList.toggle('next',i===nextQ&&!btn.classList.contains('dead'));}}
  function updateToggleUI(){for(let i=0;i<16;i++){const btn=document.getElementById('btn'+i);btn.classList.remove('active-toggle');if(!pulseTimers[i])document.getElementById('sub'+i).textContent='\u2014';if(i===activeQ){btn.classList.add('active-toggle');document.getElementById('sub'+i).textContent='ON';}}}
  function updateStatus(){
    if(Object.keys(pulseTimers).length>0)return;
    const dot=document.getElementById('status-dot');dot.className='status-dot'+(activeQ>=0?' on-toggle':'');
    document.getElementById('status-text').textContent=activeQ===-1?'All outputs OFF':'Q'+activeQ+' ON (toggle)';
  }
  function setStatusText(msg){document.getElementById('status-text').textContent=msg;}

  function pollState(){
    fetch('/state').then(r=>r.json()).then(data=>{
      activeQ=data.active; usedMask=data.used||0; twitchConn=data.twitch===true||data.twitch==='true'; nextQ=data.nextQ||0;
      if(data.adcCurr!==undefined)document.getElementById('adc-curr').textContent=data.adcCurr+'mV';
      if(data.adcMax!==undefined)document.getElementById('adc-max').textContent=data.adcMax+'mV';
      if(data.adcMin!==undefined)document.getElementById('adc-min').textContent=data.adcMin+'mV';
      if(data.ampCurr!==undefined)document.getElementById('amp-curr').textContent=data.ampCurr+'A';
      if(data.ampMax!==undefined)document.getElementById('amp-max').textContent=data.ampMax+'A';
      if(data.ampMin!==undefined)document.getElementById('amp-min').textContent=data.ampMin+'A';
      updateToggleUI(); updateDeadUI(); updateNextUI(); updateStatus();
      if(data.peaks){for(let i=0;i<16;i++){const btn=document.getElementById('btn'+i);if(data.peaks[i]>0){btn.dataset.peak=(data.peaks[i]/1000).toFixed(2)+'A';const sub=document.getElementById('sub'+i);if(!pulseTimers[i]&&i!==activeQ)sub.textContent=btn.dataset.peak;}}}
      document.getElementById('twitch-dot').className='twitch-dot'+(twitchConn?' connected':'');
    }).catch(()=>{});
  }

  let consoleSeen='', consoleLines=[];
  function pollLog(){
    fetch('/log').then(r=>r.text()).then(txt=>{
      if(txt!==consoleSeen){consoleSeen=txt;consoleLines=txt.split('\n').filter(l=>l.length>0);if(consoleLines.length>500)consoleLines=consoleLines.slice(-500);const el=document.getElementById('console');const atBottom=el.scrollHeight-el.scrollTop-el.clientHeight<40;el.textContent=consoleLines.join('\n');if(atBottom)el.scrollTop=el.scrollHeight;}
    }).catch(()=>{});
  }
  function clearConsole(){document.getElementById('console').textContent='';consoleSeen='';consoleLines=[];}

  pollState(); pollLog();
  setInterval(pollState,3000); setInterval(pollLog,1000);
</script>
</body>
</html>
)rawliteral";

// ── Config page ──────────────────────────────────────────────────────────────
const char CONFIG_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en" data-theme="dark">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>TwitchBlows &mdash; Config</title>
  <link rel="preconnect" href="https://fonts.googleapis.com">
  <link rel="preconnect" href="https://fonts.gstatic.com" crossorigin>
  <link href="https://fonts.googleapis.com/css2?family=JetBrains+Mono:wght@400;600;700&family=Inter:wght@400;500;600&display=swap" rel="stylesheet">
  <style>
    *,*::before,*::after{box-sizing:border-box;margin:0;padding:0;}
    :root{
      --bg:#0f1117; --surface:#171b24; --surface-2:#1e2330;
      --border:rgba(255,255,255,0.08);
      --text:#e2e8f0; --muted:#64748b; --faint:#334155;
      --accent:#e94560; --green:#22c55e;
      --radius:8px;
      --font-mono:'JetBrains Mono',monospace;
      --font-ui:'Inter',sans-serif;
      --tr:180ms cubic-bezier(0.16,1,0.3,1);
    }
    html,body{min-height:100dvh;background:var(--bg);color:var(--text);font-family:var(--font-ui);font-size:14px;-webkit-font-smoothing:antialiased;}
    body{display:flex;flex-direction:column;align-items:center;padding:1rem 0.75rem 3rem;gap:0.85rem;}

    header{display:flex;align-items:center;gap:0.6rem;width:100%;max-width:600px;}
    .logo-mark{width:28px;height:28px;flex-shrink:0;}
    h1{font-family:var(--font-mono);font-size:1.1rem;font-weight:700;color:var(--accent);letter-spacing:-0.02em;}
    h1 span{color:var(--text);}
    .back-link{margin-left:auto;padding:0.28rem 0.65rem;background:var(--surface-2);border:1px solid var(--border);border-radius:5px;color:var(--muted);font-size:0.72rem;font-weight:600;text-decoration:none;transition:border-color var(--tr),color var(--tr);}
    .back-link:hover{border-color:var(--accent);color:var(--accent);}

    .section{width:100%;max-width:600px;background:var(--surface);border:1px solid var(--border);border-radius:var(--radius);padding:0.9rem 1rem 1rem;display:flex;flex-direction:column;gap:0.65rem;}
    .section-title{font-size:0.68rem;font-weight:700;text-transform:uppercase;letter-spacing:0.09em;color:var(--accent);border-bottom:1px solid var(--border);padding-bottom:0.45rem;margin-bottom:0.1rem;}

    .row{display:flex;flex-direction:column;gap:0.22rem;}
    .row label{font-size:0.67rem;font-weight:600;text-transform:uppercase;letter-spacing:0.06em;color:var(--muted);}
    .row input[type="text"],.row input[type="password"],.row input[type="number"]{
      padding:0.38rem 0.55rem;background:var(--surface-2);border:1px solid var(--border);border-radius:5px;
      color:var(--text);font-family:var(--font-mono);font-size:0.8rem;
      transition:border-color var(--tr);
    }
    .row input:focus{outline:none;border-color:var(--accent);}
    .row input::-webkit-outer-spin-button,.row input::-webkit-inner-spin-button{-webkit-appearance:none;}
    .row input[type="number"]{appearance:textfield;-moz-appearance:textfield;}
    /* small number inputs */
    .row input.sm{width:80px;}
    .row input.md{width:130px;}

    .inline{display:grid;grid-template-columns:repeat(auto-fill,minmax(120px,1fr));gap:0.6rem;}

    .checks{display:flex;flex-wrap:wrap;gap:0.4rem;}
    .checks label{display:flex;align-items:center;gap:0.3rem;font-size:0.75rem;color:var(--muted);cursor:pointer;padding:0.28rem 0.55rem;background:var(--surface-2);border:1px solid var(--border);border-radius:4px;transition:border-color var(--tr),color var(--tr);font-weight:500;text-transform:none;letter-spacing:0;}
    .checks label:has(input:checked){border-color:var(--accent);color:var(--accent);}
    .checks input{display:none;}

    .hint{font-size:0.62rem;color:var(--muted);opacity:0.7;margin-top:0.1rem;}

    .actions{display:flex;gap:0.6rem;margin-top:0.3rem;width:100%;max-width:600px;}
    .save-btn{flex:1;padding:0.65rem;background:var(--accent);border:none;border-radius:var(--radius);color:#fff;font-size:0.85rem;font-weight:700;font-family:var(--font-ui);cursor:pointer;transition:opacity var(--tr);}
    .save-btn:hover{opacity:0.85;}
    .toast{position:fixed;bottom:1.5rem;left:50%;transform:translateX(-50%);background:#22c55e;color:#0f1117;font-size:0.8rem;font-weight:700;padding:0.5rem 1.2rem;border-radius:20px;opacity:0;pointer-events:none;transition:opacity 0.3s;font-family:var(--font-ui);}
    .toast.show{opacity:1;}

    @media(max-width:420px){.inline{grid-template-columns:1fr 1fr;}}
  </style>
</head>
<body>

<header>
  <svg class="logo-mark" viewBox="0 0 36 36" fill="none" xmlns="http://www.w3.org/2000/svg">
    <rect width="36" height="36" rx="8" fill="#e94560" fill-opacity="0.12"/>
    <path d="M9 12 L18 7 L27 12 L27 24 L18 29 L9 24 Z" stroke="#e94560" stroke-width="1.8" fill="none"/>
    <circle cx="18" cy="18" r="4" fill="#e94560"/>
    <path d="M18 14 L18 7 M18 22 L18 29 M14 16 L9 12 M22 20 L27 24 M14 20 L9 24 M22 16 L27 12" stroke="#e94560" stroke-width="1.4" opacity="0.5"/>
  </svg>
  <h1><span>Twitch</span>Blows &mdash; Config</h1>
  <a class="back-link" href="/">&#8592; Dashboard</a>
</header>

<!-- Twitch IRC -->
<div class="section">
  <div class="section-title">Twitch IRC</div>
  <div class="row">
    <label for="cfg-channel">Channel</label>
    <input type="text" id="cfg-channel" placeholder="channelname" class="md">
  </div>
  <div class="row">
    <label for="cfg-nick">Bot Nickname</label>
    <input type="text" id="cfg-nick" placeholder="your_bot_username" class="md">
  </div>
  <div class="row">
    <label for="cfg-oauth">OAuth Token</label>
    <input type="password" id="cfg-oauth" placeholder="oauth:... (leave blank to keep)" style="width:100%;">
    <span class="hint">Leave blank to keep the stored token.</span>
  </div>
</div>

<!-- Event Triggers -->
<div class="section">
  <div class="section-title">Event Triggers</div>
  <div class="row">
    <label>Enable</label>
    <div class="checks">
      <label><input type="checkbox" id="cb-bits"> Bits</label>
      <label><input type="checkbox" id="cb-points"> Channel Points</label>
      <label><input type="checkbox" id="cb-subs"> Subs</label>
      <label><input type="checkbox" id="cb-raids"> Raids</label>
    </div>
  </div>
  <div class="inline">
    <div class="row"><label for="cfg-bits">Bits Threshold</label><input type="number" id="cfg-bits" class="sm" value="100" min="1" max="1000000"></div>
    <div class="row"><label for="cfg-points-thresh">Points (N redemptions)</label><input type="number" id="cfg-points-thresh" class="sm" value="1" min="1" max="1000"></div>
    <div class="row"><label for="cfg-subs-thresh">Subs Threshold</label><input type="number" id="cfg-subs-thresh" class="sm" value="1" min="1" max="1000"></div>
    <div class="row"><label for="cfg-raid-thresh">Raid Min Viewers</label><input type="number" id="cfg-raid-thresh" class="sm" value="10" min="1" max="100000"></div>
  </div>
  <div class="row">
    <label for="cfg-pts-filter">Points Reward ID Filter</label>
    <input type="text" id="cfg-pts-filter" placeholder="UUID (blank = all redemptions)" style="width:100%;">
    <span class="hint">Copy from Twitch reward URL. Leave blank to fire on any redemption.</span>
  </div>
</div>

<!-- Output & Timing -->
<div class="section">
  <div class="section-title">Output &amp; Timing</div>
  <div class="inline">
    <div class="row"><label for="cfg-pulse">Pulse Duration (ms)</label><input type="number" id="cfg-pulse" class="sm" value="500" min="10" max="30000" step="10"></div>
    <div class="row"><label for="cfg-min-gap">Min Gap Between Fires (ms)</label><input type="number" id="cfg-min-gap" class="sm" value="2000" min="0" max="60000" step="100"></div>
    <div class="row"><label for="cfg-cs-delay">Current Sense Delay (ms)</label><input type="number" id="cfg-cs-delay" class="sm" value="10" min="1" max="500"></div>
  </div>
</div>

<!-- Current Sensor Calibration -->
<div class="section">
  <div class="section-title">Current Sensor Calibration</div>
  <span class="hint" style="margin-bottom:0.2rem;">ACS-style sensor on ADC pin 0. 12-bit, 3.3V reference.</span>
  <div class="inline">
    <div class="row"><label for="cfg-cs-mid">Midpoint Voltage (V)</label><input type="number" id="cfg-cs-mid" class="sm" value="2.5" min="0" max="3.3" step="0.01"><span class="hint">Idle ADC V at 0A (typ. 2.5)</span></div>
    <div class="row"><label for="cfg-cs-mva">Sensitivity (mV/A)</label><input type="number" id="cfg-cs-mva" class="sm" value="-100" min="-1000" max="1000" step="1"><span class="hint">e.g. ACS712-5A=185, 20A=100, 30A=66. Negative if inverted.</span></div>
    <div class="row"><label for="cfg-cs-detect">Detect Threshold (A)</label><input type="number" id="cfg-cs-detect" class="sm" value="3.0" min="0.01" max="50" step="0.1"><span class="hint">Min |A| to count as live output</span></div>
  </div>
</div>

<div class="actions">
  <button class="save-btn" onclick="saveCfg()">&#10003; Save Config</button>
</div>

<div class="toast" id="toast"></div>

<script>
  function showToast(msg,ok){
    const t=document.getElementById('toast');
    t.textContent=msg;
    t.style.background=ok?'#22c55e':'#ef4444';
    t.classList.add('show');
    setTimeout(()=>t.classList.remove('show'),2500);
  }

  function loadCfg(){
    fetch('/getcfg').then(r=>r.json()).then(data=>{
      document.getElementById('cfg-channel').value       = data.channel       || '';
      document.getElementById('cfg-bits').value          = data.bitsThreshold  ?? 100;
      document.getElementById('cfg-points-thresh').value = data.pointsThreshold ?? 1;
      document.getElementById('cfg-subs-thresh').value   = data.subsThreshold   ?? 1;
      document.getElementById('cfg-raid-thresh').value   = data.raidThreshold   ?? 10;
      document.getElementById('cfg-pulse').value         = data.pulseDurMs     ?? 500;
      document.getElementById('cfg-min-gap').value       = data.minGapMs       ?? 2000;
      document.getElementById('cfg-cs-delay').value      = data.csDelayMs      ?? 10;
      document.getElementById('cfg-pts-filter').value    = data.ptsFilter       || '';
      document.getElementById('cb-bits').checked         = data.evBits   === true || data.evBits   === 'true';
      document.getElementById('cb-points').checked       = data.evPoints === true || data.evPoints === 'true';
      document.getElementById('cb-subs').checked         = data.evSubs   === true || data.evSubs   === 'true';
      document.getElementById('cb-raids').checked        = data.evRaids  === true || data.evRaids  === 'true';
      document.getElementById('cfg-oauth').value         = '';
      document.getElementById('cfg-nick').value          = '';
      if(data.csMidpointV  !== undefined) document.getElementById('cfg-cs-mid').value     = data.csMidpointV;
      if(data.csMvPerAmp   !== undefined) document.getElementById('cfg-cs-mva').value     = data.csMvPerAmp;
      if(data.csDetectAmps !== undefined) document.getElementById('cfg-cs-detect').value  = data.csDetectAmps;
    }).catch(()=>showToast('Failed to load config',false));
  }

  function saveCfg(){
    const p=new URLSearchParams();
    const ch=document.getElementById('cfg-channel').value.trim();
    if(ch) p.append('channel',ch);
    p.append('bits_threshold',   document.getElementById('cfg-bits').value);
    p.append('points_threshold', document.getElementById('cfg-points-thresh').value);
    p.append('subs_threshold',   document.getElementById('cfg-subs-thresh').value);
    p.append('raid_threshold',   document.getElementById('cfg-raid-thresh').value);
    p.append('pulse_ms',         document.getElementById('cfg-pulse').value);
    p.append('min_gap_ms',       document.getElementById('cfg-min-gap').value);
    p.append('cs_delay_ms',      document.getElementById('cfg-cs-delay').value);
    p.append('cs_midpoint_v',    document.getElementById('cfg-cs-mid').value);
    p.append('cs_mv_per_amp',    document.getElementById('cfg-cs-mva').value);
    p.append('cs_detect_amps',   document.getElementById('cfg-cs-detect').value);
    const pf=document.getElementById('cfg-pts-filter').value.trim();
    p.append('pts_filter', pf);
    p.append('ev_bits',   document.getElementById('cb-bits').checked   ? '1':'0');
    p.append('ev_points', document.getElementById('cb-points').checked ? '1':'0');
    p.append('ev_subs',   document.getElementById('cb-subs').checked   ? '1':'0');
    p.append('ev_raids',  document.getElementById('cb-raids').checked  ? '1':'0');
    const oa=document.getElementById('cfg-oauth').value.trim();
    if(oa) p.append('oauth',oa);
    const nk=document.getElementById('cfg-nick').value.trim();
    if(nk) p.append('nick',nk);

    fetch('/savecfg',{method:'POST',body:p})
      .then(r=>r.json())
      .then(data=>{ if(data.ok) showToast('Saved!',true); else showToast('Save failed',false); })
      .catch(()=>showToast('Error saving',false));
  }

  loadCfg();
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
    *,*::before,*::after{box-sizing:border-box;margin:0;padding:0;}
    body{font-family:sans-serif;background:#0f1117;color:#e2e8f0;display:flex;flex-direction:column;align-items:center;padding:2rem;gap:1rem;}
    h1{color:#e94560;font-size:1.4rem;}
    form{display:flex;flex-direction:column;gap:.8rem;width:100%;max-width:340px;margin-top:1rem;}
    input{padding:.7rem;border-radius:6px;border:1px solid rgba(255,255,255,0.1);background:#171b24;color:#e2e8f0;font-size:1rem;}
    input:focus{outline:2px solid #e94560;}
    input[type=submit]{background:#e94560;border:none;cursor:pointer;font-weight:bold;}
    input[type=submit]:hover{background:#c73652;}
    p.note{font-size:.85rem;color:#64748b;text-align:center;}
  </style>
</head>
<body>
  <h1>WiFi Setup</h1>
  <p class="note">Connect to configure your network credentials.</p>
  <form action="/savewifi" method="GET">
    <input type="text" name="ssid" placeholder="SSID" required>
    <input type="password" name="psk" placeholder="Password">
    <input type="submit" value="Save &amp; Connect">
  </form>
</body>
</html>
)rawliteral";

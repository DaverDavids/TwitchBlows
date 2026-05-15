// html.h
#pragma once

const char INDEX_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width,initial-scale=1.0">
  <title>TwitchBlows</title>
  <link rel="preconnect" href="https://fonts.googleapis.com">
  <link rel="preconnect" href="https://fonts.gstatic.com" crossorigin>
  <link href="https://fonts.googleapis.com/css2?family=JetBrains+Mono:wght@400;600;700&family=Inter:wght@400;500;600&display=swap" rel="stylesheet">
  <style>

    *, *::before, *::after { box-sizing: border-box; margin: 0; padding: 0; }
    :root {
      --bg:        #0f1117;
      --surf:      #171b24;
      --surf2:     #1e2330;
      --border:    rgba(255,255,255,0.08);
      --text:      #e2e8f0;
      --muted:     #64748b;
      --faint:     #334155;
      --accent:    #e94560;
      --adim:      #e9456033;
      --green:     #22c55e;
      --red:       #ef4444;
      --amber:     #f59e0b;
      --adim2:     #f59e0b22;
      --r:         8px;
      --mono:      'JetBrains Mono','Courier New',monospace;
      --ui:        'Inter',sans-serif;
      --tr:        180ms cubic-bezier(0.16,1,0.3,1);
    }
    html,body { min-height:100dvh; background:var(--bg); color:var(--text);
      font-family:var(--ui); font-size:14px; -webkit-font-smoothing:antialiased; }
    a,button,input,select { font:inherit; color:inherit; }
    button { cursor:pointer; background:none; border:none; }
    input:focus { outline:none; }

    body { display:flex; flex-direction:column; align-items:center;
           padding:1.25rem 0.75rem 2.5rem; gap:0.9rem; }

    header { display:flex; align-items:center; gap:0.6rem;
             width:100%; max-width:560px; }
    .logo { width:30px; height:30px; flex-shrink:0; }
    h1 { font-family:var(--mono); font-size:1.2rem; font-weight:700;
         letter-spacing:-0.02em; color:var(--accent); }
    h1 span { color:var(--text); }
    .hdr-r { margin-left:auto; display:flex; align-items:center; gap:0.6rem; }

    .twitch-dot { display:flex; align-items:center; gap:0.3rem;
      font-size:0.7rem; font-weight:600; color:var(--muted); font-family:var(--mono); }
    .twitch-dot .dot { width:7px; height:7px; border-radius:50%; background:var(--faint);
      transition:background var(--tr),box-shadow var(--tr); }
    .twitch-dot.connected .dot { background:var(--green); box-shadow:0 0 5px var(--green); }
    .twitch-dot.connected { color:var(--green); }

    .cfg-link { padding:0.3rem 0.75rem; background:var(--surf2); border:1px solid var(--border);
      border-radius:5px; color:var(--muted); font-size:0.75rem; font-weight:600;
      text-decoration:none; transition:background var(--tr),color var(--tr),border-color var(--tr); }
    .cfg-link:hover { border-color:var(--accent); color:var(--accent); }

    .mode-bar { width:100%; max-width:560px; background:var(--surf); border:1px solid var(--border);
      border-radius:var(--r); padding:0.6rem 0.85rem; display:flex; align-items:center;
      gap:0.75rem; flex-wrap:wrap; }
    .mode-lbl { font-size:0.7rem; font-weight:600; text-transform:uppercase;
      letter-spacing:0.08em; color:var(--muted); white-space:nowrap; }
    .mode-tog { display:flex; background:var(--surf2); border:1px solid var(--border);
      border-radius:5px; overflow:hidden; }
    .mode-tog button { padding:0.3rem 0.75rem; font-size:0.75rem; font-weight:600;
      background:none; border:none; color:var(--muted); cursor:pointer;
      transition:background var(--tr),color var(--tr); }
    .mode-tog button.active { background:var(--accent); color:#fff; }
    .pulse-row { display:flex; align-items:center; gap:0.4rem; margin-left:auto; }
    .pulse-row label { font-size:0.75rem; color:var(--muted); white-space:nowrap; }
    .pulse-row input { width:72px; padding:0.3rem 0.4rem; background:var(--surf2);
      border:1px solid var(--border); border-radius:5px; color:var(--text);
      font-family:var(--mono); font-size:0.8rem; font-weight:600; text-align:center;
      -moz-appearance:textfield; transition:border-color var(--tr); }
    .pulse-row input:focus { border-color:var(--accent); }
    .pulse-row input::-webkit-outer-spin-button,
    .pulse-row input::-webkit-inner-spin-button { -webkit-appearance:none; }
    .pulse-row .unit { font-size:0.7rem; color:var(--muted); }

    .grid { display:grid; grid-template-columns:repeat(4,1fr);
            gap:0.6rem; width:100%; max-width:560px; }
    .obtn { position:relative; display:flex; flex-direction:column; align-items:center;
      justify-content:center; gap:0.2rem; padding:0.9rem 0.4rem 0.7rem;
      background:var(--surf); border:1.5px solid var(--border); border-radius:var(--r);
      color:var(--muted); cursor:pointer; font-family:var(--mono); font-weight:700;
      font-size:0.95rem; transition:background var(--tr),border-color var(--tr),
      color var(--tr),box-shadow var(--tr),transform 80ms ease;
      user-select:none; -webkit-tap-highlight-color:transparent; }
    .obtn:active:not(.dead) { transform:scale(0.95); }
    .obtn .qlbl { font-size:1rem; }
    .obtn .qsub { font-size:0.75rem; font-family:var(--ui); font-weight:600; opacity:0.85; }
    .obtn::before { content:''; position:absolute; top:6px; right:8px;
      width:6px; height:6px; border-radius:50%; background:var(--faint);
      transition:background var(--tr),box-shadow var(--tr); }
    .obtn.act-tog { background:var(--adim); border-color:var(--accent); color:var(--accent);
      box-shadow:0 0 14px var(--adim); }
    .obtn.act-tog::before { background:var(--accent); box-shadow:0 0 5px var(--accent); }
    .obtn.act-pul { background:var(--adim2); border-color:var(--amber); color:var(--amber);
      box-shadow:0 0 14px var(--adim2); animation:pglow .7s ease-in-out infinite alternate; }
    .obtn.act-pul::before { background:var(--amber); box-shadow:0 0 5px var(--amber); }
    .obtn.dead { border-color:var(--faint); color:var(--faint); cursor:not-allowed; opacity:0.55; }
    .obtn.dead::before { background:var(--red); opacity:0.5; }
    .obtn.nxt::after { content:'\25B6'; position:absolute; top:5px; left:6px;
      font-size:0.55rem; color:var(--green); }
    .obtn .qtog { position:absolute; bottom:3px; right:5px; font-size:0.5rem;
      cursor:pointer; opacity:0.35; transition:opacity var(--tr); background:none;
      border:none; color:var(--muted); padding:2px 3px; line-height:1; }
    .obtn .qtog:hover { opacity:1; color:var(--accent); }
    @keyframes pglow { from{box-shadow:0 0 6px var(--adim2)} to{box-shadow:0 0 18px var(--amber)} }
    .pbar { position:absolute; bottom:0; left:0; height:2px; background:var(--amber);
      border-radius:0 0 var(--r) var(--r); width:100%; transform-origin:left; }

    .bot-row { display:flex; gap:0.6rem; width:100%; max-width:560px; }
    .bot-btn { flex:1; padding:0.65rem; background:var(--surf); border:1.5px solid var(--border);
      border-radius:var(--r); color:var(--muted); font-size:0.78rem; font-weight:600;
      cursor:pointer; transition:background var(--tr),border-color var(--tr),color var(--tr); }
    .bot-btn.off:hover { border-color:var(--accent); color:var(--accent); }
    .bot-btn.rst:hover { border-color:var(--green); color:var(--green); }

    .sensor-row { display:grid; grid-template-columns:1fr 1fr;
      gap:0.6rem; width:100%; max-width:560px; }
    .sensor-card { background:var(--surf); border:1px solid var(--border);
      border-radius:var(--r); padding:0.45rem 0.75rem;
      display:flex; flex-direction:column; gap:0.2rem; }
    .sensor-ttl { font-size:0.65rem; font-weight:700; text-transform:uppercase;
      letter-spacing:0.07em; color:var(--muted); }
    .sensor-vals { display:flex; gap:0.75rem; align-items:baseline; flex-wrap:wrap; }
    .sv-group { display:flex; flex-direction:column; }
    .sv-lbl { font-size:0.62rem; color:var(--muted); }
    .sv-val { font-family:var(--mono); font-size:0.85rem; font-weight:700; color:var(--green); }

    .status-bar { width:100%; max-width:560px; background:var(--surf);
      border:1px solid var(--border); border-radius:var(--r);
      padding:0.45rem 0.85rem; display:flex; align-items:center; gap:0.5rem; }
    .sdot { width:7px; height:7px; border-radius:50%; background:var(--faint); flex-shrink:0;
      transition:background var(--tr); }
    .sdot.on-t { background:var(--accent); box-shadow:0 0 5px var(--accent); }
    .sdot.on-p { background:var(--amber);  box-shadow:0 0 5px var(--amber); }
    #status-text { font-size:0.75rem; color:var(--muted); font-family:var(--mono); }

    .con-hdr { display:flex; align-items:center; justify-content:space-between;
      width:100%; max-width:560px; font-size:0.7rem; color:var(--muted);
      font-weight:600; text-transform:uppercase; letter-spacing:0.07em; }
    .con-hdr button { font-size:0.65rem; padding:2px 7px; background:var(--surf2);
      border:1px solid var(--border); border-radius:4px; color:var(--muted); cursor:pointer; }
    .con-hdr button:hover { color:var(--text); }
    .console { width:100%; max-width:560px; background:#0a0c10;
      border:1px solid var(--border); border-radius:var(--r); padding:0.6rem;
      font-family:var(--mono); font-size:0.68rem; color:#7ee787; height:160px;
      overflow-y:auto; white-space:pre-wrap; word-break:break-all; }

    @media(max-width:400px){
      .mode-bar{flex-direction:column;align-items:flex-start;}
      .pulse-row{margin-left:0;}
      .sensor-row{grid-template-columns:1fr;}
    }
  </style>
</head>
<body>

<header>
  <svg class="logo" viewBox="0 0 30 30" fill="none" aria-label="TwitchBlows logo">
    <rect width="30" height="30" rx="7" fill="#e94560" fill-opacity="0.12"/>
    <path d="M7 10L15 6L23 10L23 20L15 24L7 20Z" stroke="#e94560" stroke-width="1.6" fill="none"/>
    <circle cx="15" cy="15" r="3.5" fill="#e94560"/>
    <path d="M15 11.5L15 6M15 18.5L15 24M11.5 13L7 10M18.5 17L23 20M11.5 17L7 20M18.5 13L23 10"
          stroke="#e94560" stroke-width="1.2" opacity="0.5"/>
  </svg>
  <h1><span>Twitch</span>Blows</h1>
  <div class="hdr-r">
    <div class="twitch-dot" id="twitch-dot">
      <div class="dot"></div><span>Twitch</span>
    </div>
    <a class="cfg-link" href="/config">&#9881; Config</a>
  </div>
</header>

<div class="mode-bar">
  <span class="mode-lbl">Mode</span>
  <div class="mode-tog">
    <button id="btn-tog" class="active" onclick="setMode('toggle')">Toggle</button>
    <button id="btn-pul"               onclick="setMode('pulse')">Pulse</button>
  </div>
  <div class="pulse-row" id="pulse-row" style="opacity:.35;pointer-events:none">
    <label for="pulse-ms">Duration</label>
    <input type="number" id="pulse-ms" value="500" min="10" max="30000" step="10">
    <span class="unit">ms</span>
  </div>
</div>

<div class="grid" id="grid"></div>

<div class="bot-row">
  <button class="bot-btn off" onclick="allOff()">&#11035; ALL OFF</button>
  <button class="bot-btn rst" onclick="resetUsed()">&#8635; RESET DEAD</button>
</div>

<div class="sensor-row">
  <div class="sensor-card">
    <div class="sensor-ttl">ADC (mV)</div>
    <div class="sensor-vals">
      <div class="sv-group"><span class="sv-lbl">NOW</span><span class="sv-val" id="adc-c">--</span></div>
      <div class="sv-group"><span class="sv-lbl">MAX</span><span class="sv-val" id="adc-x">--</span></div>
      <div class="sv-group"><span class="sv-lbl">MIN</span><span class="sv-val" id="adc-n">--</span></div>
    </div>
  </div>
  <div class="sensor-card">
    <div class="sensor-ttl">Current (A)</div>
    <div class="sensor-vals">
      <div class="sv-group"><span class="sv-lbl">NOW</span><span class="sv-val" id="amp-c">--</span></div>
      <div class="sv-group"><span class="sv-lbl">MAX</span><span class="sv-val" id="amp-x">--</span></div>
      <div class="sv-group"><span class="sv-lbl">MIN</span><span class="sv-val" id="amp-n">--</span></div>
    </div>
  </div>
</div>

<div class="status-bar">
  <div class="sdot" id="sdot"></div>
  <span id="status-text">Loading&#8230;</span>
</div>

<div class="con-hdr">
  <span>&#9679; Device Console</span>
  <button onclick="clearCon()">Clear</button>
</div>
<div class="console" id="con">Connecting...</div>

<script>
  let mode='toggle', activeQ=-1, pulseT={}, pulsB={}, usedMask=0, nextQ=0;

  const grid=document.getElementById('grid');
  for(let i=0;i<16;i++){
    const b=document.createElement('button');
    b.className='obtn'; b.id='b'+i;
    b.setAttribute('aria-label','Channel '+(i+1));
    b.innerHTML='<span class="qlbl">Ch'+(i+1)+'</span>'+
      '<span class="qsub" id="s'+i+'">—</span>'+
      '<button class="qtog" id="t'+i+'" onclick="event.stopPropagation();togChan('+i+')" title="Toggle dead">✓</button>';
    b.onclick=()=>btnClick(i);
    grid.appendChild(b);
  }

  try{
    const sm=localStorage.getItem('tb_mode'),ms=localStorage.getItem('tb_ms');
    if(sm==='toggle'||sm==='pulse')mode=sm;
    if(ms){const v=parseInt(ms);if(v>=10&&v<=30000)document.getElementById('pulse-ms').value=v;}
  }catch(e){}
  applyMode(mode);

  function savePrefs(){try{localStorage.setItem('tb_mode',mode);localStorage.setItem('tb_ms',document.getElementById('pulse-ms').value);}catch(e){}}
  document.getElementById('pulse-ms').addEventListener('change',savePrefs);
  document.getElementById('pulse-ms').addEventListener('input',savePrefs);

  function setMode(m){mode=m;applyMode(m);savePrefs();}
  function applyMode(m){
    document.getElementById('btn-tog').classList.toggle('active',m==='toggle');
    document.getElementById('btn-pul').classList.toggle('active',m==='pulse');
    const pr=document.getElementById('pulse-row');
    pr.style.opacity=m==='pulse'?'1':'.35';
    pr.style.pointerEvents=m==='pulse'?'auto':'none';
  }

  function btnClick(i){
    const b=document.getElementById('b'+i);
    if(b&&b.classList.contains('dead'))return;
    if(mode==='toggle')sendSet(activeQ===i?-1:i);
    else{const ms=Math.max(10,parseInt(document.getElementById('pulse-ms').value)||500);sendPulse(i,ms);}
  }

  function sendSet(q){fetch('/set?q='+q).then(r=>r.json()).then(d=>{activeQ=d.active;updTog();updStatus();}).catch(()=>setStTxt('Error'));}

  function sendPulse(q,ms){
    cancelPulse(q);
    fetch('/pulse?q='+q+'&ms='+ms).then(r=>r.json()).then(d=>{if(d.ok)startPulUI(q,ms);}).catch(()=>setStTxt('Error'));
  }

  function cancelPulse(q){
    if(pulseT[q]){clearTimeout(pulseT[q]);delete pulseT[q];}
    if(pulsB[q]){cancelAnimationFrame(pulsB[q].r);delete pulsB[q];}
    const b=document.getElementById('b'+q);
    if(b){b.classList.remove('act-pul');const bar=b.querySelector('.pbar');if(bar)bar.remove();document.getElementById('s'+q).textContent='—';}
  }

  function startPulUI(q,ms){
    const b=document.getElementById('b'+q);
    b.classList.add('act-pul');
    let bar=b.querySelector('.pbar');
    if(!bar){bar=document.createElement('div');bar.className='pbar';b.appendChild(bar);}
    const t0=performance.now();
    function tick(now){
      const p=Math.min((now-t0)/ms,1);
      bar.style.transform='scaleX('+(1-p)+')';
      document.getElementById('s'+q).textContent=Math.max(0,(ms-(now-t0))/1000).toFixed(1)+'s';
      if(p<1)pulsB[q]={r:requestAnimationFrame(tick)};
    }
    pulsB[q]={r:requestAnimationFrame(tick)};
    document.getElementById('sdot').className='sdot on-p';
    setStTxt('Ch'+(q+1)+' pulsing '+ms+'ms');
    pulseT[q]=setTimeout(()=>{cancelPulse(q);updStatus();},ms+300);
  }

  function togChan(q){
    const dead=(usedMask&(1<<q))!==0;
    fetch('/setdead?q='+q+'&dead='+(dead?'0':'1')).then(r=>r.json()).then(d=>{
      if(d.ok){dead?(usedMask&=~(1<<q)):(usedMask|=(1<<q));updDead();}
    }).catch(()=>{});
  }

  function allOff(){
    for(let i=0;i<16;i++)cancelPulse(i);
    sendSet(-1);
    fetch('/disableall').then(r=>r.json()).then(d=>{if(d.ok){usedMask=0xFFFF;updDead();setStTxt('All OFF');}}).catch(()=>{});
  }

  function resetUsed(){
    fetch('/resetused').then(r=>r.json()).then(d=>{if(d.ok){usedMask=0;updDead();setStTxt('Dead outputs reset');}}).catch(()=>{});
  }

  function updDead(){
    for(let i=0;i<16;i++){
      const b=document.getElementById('b'+i),t=document.getElementById('t'+i);
      const d=(usedMask&(1<<i))!==0;
      if(b)b.classList.toggle('dead',d);
      if(t)t.textContent=d?'✗':'✓';
    }
  }

  function updNextUI(){for(let i=0;i<16;i++){const b=document.getElementById('b'+i);if(b)b.classList.toggle('nxt',i===nextQ&&!b.classList.contains('dead'));}}

  function updTog(){
    for(let i=0;i<16;i++){
      const b=document.getElementById('b'+i);
      b.classList.remove('act-tog');
      if(!pulseT[i])document.getElementById('s'+i).textContent='—';
      if(i===activeQ){b.classList.add('act-tog');document.getElementById('s'+i).textContent='ON';}
    }
  }

  function updStatus(){
    if(Object.keys(pulseT).length>0)return;
    document.getElementById('sdot').className='sdot'+(activeQ>=0?' on-t':'');
    setStTxt(activeQ===-1?'All outputs OFF':'Ch'+(activeQ+1)+' ON (toggle)');
  }

  function setStTxt(t){document.getElementById('status-text').textContent=t;}

  function pollStatus(){
    fetch('/status').then(r=>r.json()).then(d=>{
      activeQ=d.active; usedMask=d.usedMask||0; nextQ=d.nextQ||0;
      updTog(); updDead(); updNextUI();
      if(Object.keys(pulseT)===0)updStatus();
      document.getElementById('twitch-dot').classList.toggle('connected',!!d.twitchConnected);
      if(d.adcMv!==undefined){
        document.getElementById('adc-c').textContent=d.adcMv+'mV';
        document.getElementById('adc-x').textContent=d.adcMvMax+'mV';
        document.getElementById('adc-n').textContent=d.adcMvMin+'mV';
        document.getElementById('amp-c').textContent=d.amps!==undefined?d.amps.toFixed(2)+'A':'--';
        document.getElementById('amp-x').textContent=d.ampsMax!==undefined?d.ampsMax.toFixed(2)+'A':'--';
        document.getElementById('amp-n').textContent=d.ampsMin!==undefined?d.ampsMin.toFixed(2)+'A':'--';
      }
    }).catch(()=>{});
  }

  function pollLog(){
    fetch('/log').then(r=>r.json()).then(d=>{
      if(d.lines&&d.lines.length){
        const c=document.getElementById('con');
        d.lines.forEach(l=>{c.textContent+=l+'\n';});
        c.scrollTop=c.scrollHeight;
      }
    }).catch(()=>{});
  }

  function clearCon(){document.getElementById('con').textContent='';}

  setInterval(pollStatus,1000);
  setInterval(pollLog,2000);
  pollStatus();
</script>
</body>
</html>
)rawliteral";

const char CONFIG_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width,initial-scale=1.0">
  <title>TwitchBlows Config</title>
  <link rel="preconnect" href="https://fonts.googleapis.com">
  <link rel="preconnect" href="https://fonts.gstatic.com" crossorigin>
  <link href="https://fonts.googleapis.com/css2?family=JetBrains+Mono:wght@400;600;700&family=Inter:wght@400;500;600&display=swap" rel="stylesheet">
  <style>

    *, *::before, *::after { box-sizing: border-box; margin: 0; padding: 0; }
    :root {
      --bg:        #0f1117;
      --surf:      #171b24;
      --surf2:     #1e2330;
      --border:    rgba(255,255,255,0.08);
      --text:      #e2e8f0;
      --muted:     #64748b;
      --faint:     #334155;
      --accent:    #e94560;
      --adim:      #e9456033;
      --green:     #22c55e;
      --red:       #ef4444;
      --amber:     #f59e0b;
      --adim2:     #f59e0b22;
      --r:         8px;
      --mono:      'JetBrains Mono','Courier New',monospace;
      --ui:        'Inter',sans-serif;
      --tr:        180ms cubic-bezier(0.16,1,0.3,1);
    }
    html,body { min-height:100dvh; background:var(--bg); color:var(--text);
      font-family:var(--ui); font-size:14px; -webkit-font-smoothing:antialiased; }
    a,button,input,select { font:inherit; color:inherit; }
    button { cursor:pointer; background:none; border:none; }
    input:focus { outline:none; }

    body { display:flex; flex-direction:column; align-items:center;
           padding:1.25rem 0.75rem 2.5rem; gap:0.9rem; }

    header { display:flex; align-items:center; gap:0.6rem; width:100%; max-width:500px; }
    .logo { width:26px; height:26px; flex-shrink:0; }
    h1 { font-family:var(--mono); font-size:1.1rem; font-weight:700;
         letter-spacing:-0.02em; color:var(--accent); }
    h1 span { color:var(--text); }
    .back-link { margin-left:auto; padding:0.3rem 0.7rem; background:var(--surf2);
      border:1px solid var(--border); border-radius:5px; color:var(--muted);
      font-size:0.72rem; font-weight:600; text-decoration:none;
      transition:color var(--tr),border-color var(--tr); }
    .back-link:hover { color:var(--text); border-color:var(--muted); }

    .card { width:100%; max-width:500px; background:var(--surf);
      border:1px solid var(--border); border-radius:var(--r);
      padding:1rem 1.1rem; display:flex; flex-direction:column; gap:0.75rem; }
    .card-ttl { font-size:0.7rem; font-weight:700; text-transform:uppercase;
      letter-spacing:0.1em; color:var(--accent); font-family:var(--mono);
      border-bottom:1px solid var(--border); padding-bottom:0.5rem; margin-bottom:0.1rem; }

    .field { display:flex; flex-direction:column; gap:0.22rem; }
    .field label { font-size:0.68rem; font-weight:600; text-transform:uppercase;
      letter-spacing:0.06em; color:var(--muted); }
    .field input[type="text"],
    .field input[type="password"],
    .field input[type="number"] {
      padding:0.35rem 0.6rem; background:var(--surf2); border:1px solid var(--border);
      border-radius:5px; color:var(--text); font-family:var(--mono); font-size:0.82rem;
      transition:border-color var(--tr); }
    .field input:focus { border-color:var(--accent); }
    .field input[type="number"] { width:120px; -moz-appearance:textfield; }
    .field input::-webkit-outer-spin-button,
    .field input::-webkit-inner-spin-button { -webkit-appearance:none; }
    .field .hint { font-size:0.62rem; color:var(--muted); margin-top:0.05rem; }

    .row2 { display:grid; grid-template-columns:1fr 1fr; gap:0.75rem; }
    .row3 { display:grid; grid-template-columns:1fr 1fr 1fr; gap:0.75rem; }

    .chips { display:flex; flex-wrap:wrap; gap:0.4rem; }
    .chips label { display:flex; align-items:center; gap:0.3rem;
      font-size:0.75rem; color:var(--muted); cursor:pointer;
      padding:0.28rem 0.55rem; background:var(--surf2); border:1px solid var(--border);
      border-radius:4px; transition:border-color var(--tr),color var(--tr);
      text-transform:none; letter-spacing:0; font-weight:500; }
    .chips label:has(input:checked) { border-color:var(--accent); color:var(--accent); }
    .chips input[type="checkbox"] { display:none; }

    .actions { display:flex; gap:0.6rem; width:100%; max-width:500px; margin-top:0.25rem; }
    .save-btn { flex:1; padding:0.7rem; background:var(--accent); border:none;
      border-radius:var(--r); color:#fff; font-size:0.88rem; font-weight:700;
      cursor:pointer; transition:opacity var(--tr); }
    .save-btn:hover { opacity:0.85; }
    .toast { display:none; align-items:center; justify-content:center;
      width:100%; max-width:500px; padding:0.5rem; background:var(--surf);
      border:1px solid var(--green); border-radius:var(--r);
      color:var(--green); font-family:var(--mono); font-size:0.78rem; font-weight:600; }
    .toast.visible { display:flex; }

    @media(max-width:400px){ .row2,.row3{grid-template-columns:1fr;} }
  </style>
</head>
<body>

<header>
  <svg class="logo" viewBox="0 0 26 26" fill="none" aria-label="TwitchBlows logo">
    <rect width="26" height="26" rx="6" fill="#e94560" fill-opacity="0.12"/>
    <path d="M5 8.5L13 4.5L21 8.5L21 17.5L13 21.5L5 17.5Z" stroke="#e94560" stroke-width="1.5" fill="none"/>
    <circle cx="13" cy="13" r="3" fill="#e94560"/>
  </svg>
  <h1><span>Twitch</span>Blows &mdash; Config</h1>
  <a class="back-link" href="/">&larr; Dashboard</a>
</header>

<div class="card">
  <div class="card-ttl">Twitch IRC</div>
  <div class="field">
    <label for="cfg-ch">Channel</label>
    <input type="text" id="cfg-ch" placeholder="yourchannel">
  </div>
  <div class="field">
    <label for="cfg-nick">Bot Nickname</label>
    <input type="text" id="cfg-nick" placeholder="your_bot_username">
  </div>
  <div class="field">
    <label for="cfg-oauth">OAuth Token</label>
    <input type="password" id="cfg-oauth" placeholder="oauth:xxxxxxxxxxxxxx">
    <span class="hint">Generate at twitchapps.com/tmi</span>
  </div>
</div>

<div class="card">
  <div class="card-ttl">Event Triggers</div>
  <div class="field">
    <label>Enable Events</label>
    <div class="chips">
      <label><input type="checkbox" id="cb-bits"> Bits</label>
      <label><input type="checkbox" id="cb-points"> Channel Points</label>
      <label><input type="checkbox" id="cb-subs"> Subs</label>
      <label><input type="checkbox" id="cb-raids"> Raids</label>
    </div>
  </div>
  <div class="row2">
    <div class="field">
      <label for="cfg-bits">Bits Threshold</label>
      <input type="number" id="cfg-bits" value="100" min="1" max="1000000">
    </div>
    <div class="field">
      <label for="cfg-pts">Points Threshold</label>
      <input type="number" id="cfg-pts" value="1" min="1" max="1000">
    </div>
  </div>
  <div class="row2">
    <div class="field">
      <label for="cfg-subs">Subs Threshold</label>
      <input type="number" id="cfg-subs" value="1" min="1" max="1000">
    </div>
    <div class="field">
      <label for="cfg-raid">Raid Viewer Min</label>
      <input type="number" id="cfg-raid" value="10" min="1" max="100000">
    </div>
  </div>
  <div class="field">
    <label for="cfg-ptsfilter">Channel Points Reward ID Filter</label>
    <input type="text" id="cfg-ptsfilter" placeholder="blank = fire on any redemption">
    <span class="hint">Paste a specific reward UUID to filter, or leave blank for all</span>
  </div>
</div>

<div class="card">
  <div class="card-ttl">Output &amp; Timing</div>
  <div class="row3">
    <div class="field">
      <label for="cfg-pulse">Pulse Duration</label>
      <input type="number" id="cfg-pulse" value="500" min="10" max="30000" step="10">
      <span class="hint">ms</span>
    </div>
    <div class="field">
      <label for="cfg-gap">Min Fire Gap</label>
      <input type="number" id="cfg-gap" value="500" min="0" max="60000" step="50">
      <span class="hint">ms between fires</span>
    </div>
    <div class="field">
      <label for="cfg-csdelay">CS Sample Delay</label>
      <input type="number" id="cfg-csdelay" value="10" min="1" max="500">
      <span class="hint">ms after fire</span>
    </div>
  </div>
</div>

<div class="card">
  <div class="card-ttl">Current Sensor Calibration</div>
  <div class="row3">
    <div class="field">
      <label for="cfg-csmid">Midpoint (V)</label>
      <input type="number" id="cfg-csmid" value="2.5" min="0" max="3.3" step="0.001">
      <span class="hint">Idle output V</span>
    </div>
    <div class="field">
      <label for="cfg-csmvpa">Sensitivity (mV/A)</label>
      <input type="number" id="cfg-csmvpa" value="-100" min="-1000" max="1000" step="0.1">
      <span class="hint">Neg = inverted</span>
    </div>
    <div class="field">
      <label for="cfg-csdet">Detect Threshold (A)</label>
      <input type="number" id="cfg-csdet" value="3.0" min="0.01" max="50" step="0.01">
      <span class="hint">Min A = live</span>
    </div>
  </div>
</div>

<div class="actions">
  <button class="save-btn" onclick="saveCfg()">&#10003; Save Config</button>
</div>
<div class="toast" id="toast">&#10003; Config saved</div>

<script>
  function loadCfg(){
    fetch('/getcfg').then(r=>r.json()).then(d=>{
      if(d.channel)  document.getElementById('cfg-ch').value=d.channel;
      if(d.nick)     document.getElementById('cfg-nick').value=d.nick;
      document.getElementById('cb-bits').checked   = !!d.evBits;
      document.getElementById('cb-points').checked = !!d.evPoints;
      document.getElementById('cb-subs').checked   = !!d.evSubs;
      document.getElementById('cb-raids').checked  = !!d.evRaids;
      if(d.bitsThreshold)   document.getElementById('cfg-bits').value=d.bitsThreshold;
      if(d.pointsThreshold) document.getElementById('cfg-pts').value=d.pointsThreshold;
      if(d.subsThreshold)   document.getElementById('cfg-subs').value=d.subsThreshold;
      if(d.raidThreshold)   document.getElementById('cfg-raid').value=d.raidThreshold;
      if(d.ptsFilter!==undefined) document.getElementById('cfg-ptsfilter').value=d.ptsFilter;
      if(d.pulseDurMs)      document.getElementById('cfg-pulse').value=d.pulseDurMs;
      if(d.minGapMs!==undefined)  document.getElementById('cfg-gap').value=d.minGapMs;
      if(d.csDelayMs)       document.getElementById('cfg-csdelay').value=d.csDelayMs;
      if(d.csMidpointV!==undefined)  document.getElementById('cfg-csmid').value=parseFloat(d.csMidpointV).toFixed(4);
      if(d.csMvPerAmp!==undefined)   document.getElementById('cfg-csmvpa').value=parseFloat(d.csMvPerAmp).toFixed(2);
      if(d.csDetectAmps!==undefined) document.getElementById('cfg-csdet').value=parseFloat(d.csDetectAmps).toFixed(4);
    }).catch(()=>{});
  }

  function saveCfg(){
    const p=new URLSearchParams();
    p.set('channel',   document.getElementById('cfg-ch').value.trim());
    p.set('nick',      document.getElementById('cfg-nick').value.trim());
    const oa=document.getElementById('cfg-oauth').value.trim();
    if(oa)p.set('oauth',oa);
    p.set('ev_bits',   document.getElementById('cb-bits').checked?'1':'0');
    p.set('ev_points', document.getElementById('cb-points').checked?'1':'0');
    p.set('ev_subs',   document.getElementById('cb-subs').checked?'1':'0');
    p.set('ev_raids',  document.getElementById('cb-raids').checked?'1':'0');
    p.set('bits_threshold',    document.getElementById('cfg-bits').value);
    p.set('points_threshold',  document.getElementById('cfg-pts').value);
    p.set('subs_threshold',    document.getElementById('cfg-subs').value);
    p.set('raid_threshold',    document.getElementById('cfg-raid').value);
    p.set('pts_reward_filter', document.getElementById('cfg-ptsfilter').value.trim());
    p.set('pulse_dur_ms',      document.getElementById('cfg-pulse').value);
    p.set('min_gap_ms',        document.getElementById('cfg-gap').value);
    p.set('cs_delay_ms',       document.getElementById('cfg-csdelay').value);
    p.set('cs_midpoint_v',     document.getElementById('cfg-csmid').value);
    p.set('cs_mv_per_amp',     document.getElementById('cfg-csmvpa').value);
    p.set('cs_detect_amps',    document.getElementById('cfg-csdet').value);
    fetch('/savecfg?'+p.toString()).then(r=>r.json()).then(d=>{
      if(d.ok){
        const t=document.getElementById('toast');
        t.classList.add('visible');
        setTimeout(()=>t.classList.remove('visible'),2500);
      }
    }).catch(()=>alert('Save failed'));
  }

  loadCfg();
</script>
</body>
</html>
)rawliteral";

const char PORTAL_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html><html><head><meta charset='UTF-8'><meta name='viewport' content='width=device-width,initial-scale=1'><title>TwitchBlows WiFi Setup</title><style>*{box-sizing:border-box;margin:0;padding:0}body{background:#0f1117;color:#e2e8f0;font-family:sans-serif;display:flex;align-items:center;justify-content:center;min-height:100vh;padding:1rem}.card{background:#171b24;border:1px solid rgba(255,255,255,.08);border-radius:10px;padding:1.5rem;width:100%;max-width:340px;display:flex;flex-direction:column;gap:1rem}h2{font-size:1.1rem;font-weight:700;color:#e94560}label{font-size:.75rem;font-weight:600;text-transform:uppercase;letter-spacing:.06em;color:#64748b;display:block;margin-bottom:.3rem}input{width:100%;padding:.45rem .7rem;background:#1e2330;border:1px solid rgba(255,255,255,.08);border-radius:6px;color:#e2e8f0;font-size:.9rem}input:focus{outline:none;border-color:#e94560}button{padding:.7rem;background:#e94560;border:none;border-radius:8px;color:#fff;font-weight:700;font-size:.9rem;cursor:pointer}</style></head><body><div class='card'><h2>TwitchBlows WiFi Setup</h2><div><label>SSID</label><input type='text' id='s' placeholder='Network name'></div><div><label>Password</label><input type='password' id='p' placeholder='Password'></div><button onclick="location.href='/save?ssid='+encodeURIComponent(document.getElementById('s').value)+'&pass='+encodeURIComponent(document.getElementById('p').value)">Connect</button></div></body></html>
)rawliteral";
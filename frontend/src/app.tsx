import { Component, createSignal, createMemo, Show, For } from 'solid-js';
import { useStore } from './store';
import { LedMatrix } from './components/LedMatrix';
import { rotateArray } from './helpers';

export const App: Component = () => {
  const store = useStore();
  const [activeTab, setActiveTab] = createSignal<'effects' | 'matrix' | 'sensors' | 'ticker'>('effects');
  const [tickerText, setTickerText] = createSignal('');
  const [tickerRepeat, setTickerRepeat] = createSignal(1);
  const [tickerSpeed, setTickerSpeed] = createSpeedSignal(30);

  function setTickerSpeedSignal(val: number) {
    setTickerSpeed(val);
  }

  const rotatedMatrix = createMemo(() =>
    rotateArray(store!.indexMatrix(), store!.rotation())
  );

  const wsMessage = (event: string, data?: any) => {
    store?.send(JSON.stringify({ event, ...data }));
  };

  const sendPlugin = (id: number) => {
    wsMessage('plugin', { plugin: id });
  };

  const sendBrightness = (val: number) => {
    store?.setBrightness(val);
    wsMessage('brightness', { brightness: val });
  };

  const rotate = (turnRight = false) => {
    let cur = store?.rotation() || 0;
    cur = turnRight ? (cur + 1) % 4 : (cur + 3) % 4;
    store?.setRotation(cur);
    wsMessage('rotate', { direction: turnRight ? 'right' : 'left' });
  };

  const setExactRotation = (rot: number) => {
    store?.setRotation(rot);
    fetch(`/api/rotation?value=${rot}`)
      .then(() => store?.toast(`Ausrichtung: ${rot * 90}°`, 1200))
      .catch((e) => store?.toast(`Fehler: ${e}`, 2000));
  };

  const sendTicker = () => {
    if (!tickerText().trim()) return;
    const url = `/message?text=${encodeURIComponent(tickerText())}&repeat=${tickerRepeat()}&delay=${tickerSpeed()}`;
    fetch(url)
      .then(() => {
        store?.toast(`Nachricht gesendet!`, 1500);
        setTickerText('');
      })
      .catch((err) => store?.toast(`Fehler: ${err}`, 2000));
  };

  const stopTicker = () => {
    fetch('/api/ticker/stop')
      .then(() => store?.toast('Ticker gestoppt', 1200))
      .catch((err) => store?.toast(`Fehler: ${err}`, 2000));
  };

  const sendTransitDemo = () => {
    fetch('/api/transit?line1=U1&min1=3&line2=BUS12&min2=8&activate=1')
      .then(() => store?.toast('ÖPNV Ansicht aktiviert (U1: 3m, BUS 12: 8m)', 2000))
      .catch((err) => store?.toast(`Fehler: ${err}`, 2000));
  };

  const sendClusterDemo = () => {
    fetch('/api/cluster?threats=0&activate=1')
      .then(() => store?.toast('Cluster Ansicht aktiviert (5 Nodes OK)', 2000))
      .catch((err) => store?.toast(`Fehler: ${err}`, 2000));
  };

  const sendAlertDemo = () => {
    fetch('/api/alert?title=SSH-BruteForce+192.168.1.105+geblockt&source=Sentinel')
      .then(() => store?.toast('Sentinel Alarm ausgelöst!', 2000))
      .catch((err) => store?.toast(`Fehler: ${err}`, 2000));
  };

  const formatUptime = (seconds?: number) => {
    if (!seconds) return '0s';
    const d = Math.floor(seconds / 86400);
    const h = Math.floor((seconds % 86400) / 3600);
    const m = Math.floor((seconds % 3600) / 60);
    const s = seconds % 60;
    if (d > 0) return `${d}d ${h}h ${m}m`;
    if (h > 0) return `${h}h ${m}m ${s}s`;
    return `${m}m ${s}s`;
  };

  const getPluginIcon = (name: string) => {
    const n = name.toLowerCase();
    if (n.includes('matrix')) return '💻';
    if (n.includes('transit')) return '🚆';
    if (n.includes('cluster')) return '🖥️';
    if (n.includes('river')) return '🌊';
    if (n.includes('air')) return '🫁';
    if (n.includes('energy')) return '⚡';
    if (n.includes('iss') || n.includes('space')) return '🛰️';
    if (n.includes('temp')) return '🌡️';
    if (n.includes('hall')) return '🧲';
    if (n.includes('wifi')) return '📶';
    if (n.includes('sensor')) return '🎛️';
    if (n.includes('clock') || n.includes('pong')) return '⏰';
    if (n.includes('pacman') || n.includes('snake') || n.includes('breakout')) return '👾';
    if (n.includes('life')) return '🧬';
    if (n.includes('heart')) return '❤️';
    if (n.includes('spectrum')) return '🎵';
    if (n.includes('rain')) return '🌧️';
    if (n.includes('firework')) return '🎆';
    if (n.includes('star')) return '✨';
    if (n.includes('weather')) return '⛅';
    if (n.includes('draw')) return '✏️';
    return '💡';
  };

  return (
    <div style={{
      "min-height": "100vh",
      "background": "#0b0f19",
      "color": "#e2e8f0",
      "font-family": "system-ui, -apple-system, sans-serif",
      "display": "flex",
      "flex-direction": "column"
    }}>
      {/* Top Header */}
      <header style={{
        "background": "#111827",
        "border-bottom": "1px solid #1f2937",
        "padding": "12px 20px",
        "display": "flex",
        "align-items": "center",
        "justify-content": "space-between",
        "flex-wrap": "wrap",
        "gap": "10px"
      }}>
        <div style={{ "display": "flex", "align-items": "center", "gap": "10px" }}>
          <span style={{ "font-size": "1.5rem" }}>⬛</span>
          <div>
            <div style={{ "font-weight": "800", "font-size": "1.1rem", "letter-spacing": "0.5px" }}>
              OBEGRÄNSAD 16×16
            </div>
            <div style={{ "font-size": "0.75rem", "color": "#9ca3af" }}>
              IKEA LED Matrix · ESP32 Smart Wall Lamp
            </div>
          </div>
        </div>

        <div style={{ "display": "flex", "align-items": "center", "gap": "12px" }}>
          <div style={{
            "display": "inline-flex",
            "align-items": "center",
            "gap": "6px",
            "background": store?.connectionState() === 1 ? "rgba(16, 185, 129, 0.15)" : "rgba(239, 68, 68, 0.15)",
            "color": store?.connectionState() === 1 ? "#34d399" : "#f87171",
            "padding": "4px 10px",
            "border-radius": "9999px",
            "font-size": "0.8rem",
            "font-weight": "600",
            "border": `1px solid ${store?.connectionState() === 1 ? "#059669" : "#dc2626"}`
          }}>
            <span style={{
              "width": "8px",
              "height": "8px",
              "border-radius": "50%",
              "background": store?.connectionState() === 1 ? "#10b981" : "#ef4444"
            }} />
            {store?.connectionState() === 1 ? "ONLINE" : store?.connectionStatus || "DISCONNECTED"}
          </div>

          <a
            href="#/creator"
            style={{
              "background": "#374151",
              "color": "#f3f4f6",
              "text-decoration": "none",
              "padding": "6px 12px",
              "border-radius": "6px",
              "font-size": "0.8rem",
              "font-weight": "600"
            }}
          >
            Studio
          </a>
        </div>
      </header>

      {/* Navigation Tabs */}
      <nav style={{
        "background": "#131b2e",
        "border-bottom": "1px solid #1e293b",
        "display": "flex",
        "justify-content": "center",
        "overflow-x": "auto",
        "padding": "6px 10px",
        "gap": "6px"
      }}>
        <button
          onClick={() => setActiveTab('effects')}
          style={{
            "background": activeTab() === 'effects' ? "#2563eb" : "transparent",
            "color": activeTab() === 'effects' ? "#ffffff" : "#94a3b8",
            "border": "none",
            "padding": "8px 16px",
            "border-radius": "6px",
            "cursor": "pointer",
            "font-weight": "600",
            "font-size": "0.85rem",
            "display": "flex",
            "align-items": "center",
            "gap": "6px"
          }}
        >
          🎛️ Steuerung & Modi
        </button>

        <button
          onClick={() => setActiveTab('matrix')}
          style={{
            "background": activeTab() === 'matrix' ? "#2563eb" : "transparent",
            "color": activeTab() === 'matrix' ? "#ffffff" : "#94a3b8",
            "border": "none",
            "padding": "8px 16px",
            "border-radius": "6px",
            "cursor": "pointer",
            "font-weight": "600",
            "font-size": "0.85rem",
            "display": "flex",
            "align-items": "center",
            "gap": "6px"
          }}
        >
          ✏️ Zeichen-Matrix
        </button>

        <button
          onClick={() => setActiveTab('sensors')}
          style={{
            "background": activeTab() === 'sensors' ? "#2563eb" : "transparent",
            "color": activeTab() === 'sensors' ? "#ffffff" : "#94a3b8",
            "border": "none",
            "padding": "8px 16px",
            "border-radius": "6px",
            "cursor": "pointer",
            "font-weight": "600",
            "font-size": "0.85rem",
            "display": "flex",
            "align-items": "center",
            "gap": "6px"
          }}
        >
          📊 ESP32 Sensoren
        </button>

        <button
          onClick={() => setActiveTab('ticker')}
          style={{
            "background": activeTab() === 'ticker' ? "#2563eb" : "transparent",
            "color": activeTab() === 'ticker' ? "#ffffff" : "#94a3b8",
            "border": "none",
            "padding": "8px 16px",
            "border-radius": "6px",
            "cursor": "pointer",
            "font-weight": "600",
            "font-size": "0.85rem",
            "display": "flex",
            "align-items": "center",
            "gap": "6px"
          }}
        >
          💬 Lauftext Ticker
        </button>
      </nav>

      {/* Main Content Area */}
      <main style={{ "flex": "1", "max-width": "900px", "margin": "0 auto", "width": "100%", "padding": "20px" }}>

        {/* Global Quick Controls (Brightness & Rotation) */}
        <div style={{
          "background": "#1e293b",
          "border": "1px solid #334155",
          "border-radius": "12px",
          "padding": "16px",
          "margin-bottom": "20px",
          "display": "flex",
          "align-items": "center",
          "justify-content": "space-between",
          "flex-wrap": "wrap",
          "gap": "14px"
        }}>
          {/* Brightness */}
          <div style={{ "flex": "1", "min-width": "240px", "display": "flex", "align-items": "center", "gap": "12px" }}>
            <span style={{ "font-size": "1.2rem" }}>☀️</span>
            <div style={{ "flex": "1" }}>
              <div style={{ "display": "flex", "justify-content": "space-between", "font-size": "0.8rem", "margin-bottom": "4px" }}>
                <span>Helligkeit</span>
                <span style={{ "font-weight": "700", "color": "#38bdf8" }}>
                  {Math.round(((store?.brightness() || 0) / 255) * 100)}%
                </span>
              </div>
              <input
                type="range"
                min="0"
                max="255"
                value={store?.brightness() || 0}
                onInput={(e) => sendBrightness(parseInt(e.currentTarget.value))}
                style={{ "width": "100%", "accent-color": "#38bdf8", "cursor": "pointer" }}
              />
            </div>
          </div>

          {/* Rotation & Quick Orientation */}
          <div style={{ "display": "flex", "align-items": "center", "gap": "6px", "flex-wrap": "wrap" }}>
            <span style={{ "font-size": "0.8rem", "color": "#94a3b8", "margin-right": "4px" }}>
              Drehung:
            </span>
            <button
              onClick={() => rotate(false)}
              title="90° gegen den Uhrzeigersinn"
              style={{
                "background": "#334155",
                "color": "#fff",
                "border": "none",
                "padding": "6px 10px",
                "border-radius": "6px",
                "cursor": "pointer",
                "font-size": "0.9rem"
              }}
            >
              ↺
            </button>
            <button
              onClick={() => rotate(true)}
              title="90° im Uhrzeigersinn"
              style={{
                "background": "#334155",
                "color": "#fff",
                "border": "none",
                "padding": "6px 10px",
                "border-radius": "6px",
                "cursor": "pointer",
                "font-size": "0.9rem"
              }}
            >
              ↻
            </button>

            {[0, 1, 2, 3].map((r) => (
              <button
                onClick={() => setExactRotation(r)}
                style={{
                  "background": (store?.rotation() || 0) === r ? "#2563eb" : "#1e293b",
                  "color": (store?.rotation() || 0) === r ? "#fff" : "#94a3b8",
                  "border": "1px solid #334155",
                  "padding": "6px 10px",
                  "border-radius": "6px",
                  "cursor": "pointer",
                  "font-size": "0.75rem",
                  "font-weight": "600"
                }}
              >
                {r * 90}°
              </button>
            ))}

            <button
              onClick={() => {
                wsMessage('persist-plugin');
                store?.toast('Aktueller Modus als Standard gespeichert', 1500);
              }}
              style={{
                "background": "#059669",
                "color": "#fff",
                "border": "none",
                "padding": "6px 12px",
                "border-radius": "6px",
                "cursor": "pointer",
                "font-size": "0.75rem",
                "font-weight": "600",
                "margin-left": "auto"
              }}
            >
              💾 Standard speichern
            </button>
          </div>
        </div>

        {/* TAB 1: Steuerung & Effekte */}
        <Show when={activeTab() === 'effects'}>
          <div>
            <div style={{ "margin-bottom": "12px", "display": "flex", "align-items": "center", "justify-content": "space-between" }}>
              <h2 style={{ "font-size": "1.1rem", "font-weight": "700", "margin": "0" }}>
                Animationen & Uhren ({store?.plugins().length || 0})
              </h2>
              <span style={{ "font-size": "0.8rem", "color": "#94a3b8" }}>
                Aktiv: <b>{store?.plugins().find(p => p.id === store?.plugin())?.name || "Modus"}</b>
              </span>
            </div>

            <div style={{
              "display": "grid",
              "grid-template-columns": "repeat(auto-fill, minmax(180px, 1fr))",
              "gap": "10px"
            }}>
              <For each={store?.plugins()}>
                {(plugin) => {
                  const isActive = () => store?.plugin() === plugin.id;
                  return (
                    <button
                      onClick={() => sendPlugin(plugin.id)}
                      style={{
                        "background": isActive() ? "#1d4ed8" : "#1e293b",
                        "border": `1px solid ${isActive() ? "#60a5fa" : "#334155"}`,
                        "color": "#fff",
                        "padding": "14px 12px",
                        "border-radius": "10px",
                        "cursor": "pointer",
                        "display": "flex",
                        "align-items": "center",
                        "gap": "10px",
                        "text-align": "left",
                        "transition": "all 0.15s ease",
                        "box-shadow": isActive() ? "0 0 12px rgba(37, 99, 235, 0.4)" : "none"
                      }}
                    >
                      <span style={{ "font-size": "1.3rem" }}>{getPluginIcon(plugin.name)}</span>
                      <div>
                        <div style={{ "font-weight": "700", "font-size": "0.9rem" }}>
                          {plugin.name}
                        </div>
                        <div style={{ "font-size": "0.7rem", "color": isActive() ? "#bfdbfe" : "#94a3b8" }}>
                          {isActive() ? "● Aktiv" : "Umschalten"}
                        </div>
                      </div>
                    </button>
                  );
                }}
              </For>
            </div>
          </div>
        </Show>

        {/* TAB 2: Zeichen-Matrix */}
        <Show when={activeTab() === 'matrix'}>
          <div style={{
            "background": "#1e293b",
            "border": "1px solid #334155",
            "border-radius": "12px",
            "padding": "20px",
            "display": "flex",
            "flex-direction": "column",
            "align-items": "center"
          }}>
            <div style={{ "margin-bottom": "14px", "display": "flex", "gap": "10px", "flex-wrap": "wrap" }}>
              <button
                onClick={() => sendPlugin(1)}
                style={{
                  "background": store?.plugin() === 1 ? "#2563eb" : "#334155",
                  "color": "#fff",
                  "border": "none",
                  "padding": "8px 14px",
                  "border-radius": "6px",
                  "cursor": "pointer",
                  "font-size": "0.85rem",
                  "font-weight": "600"
                }}
              >
                ✏️ Zeichenmodus {store?.plugin() === 1 ? "(Aktiv)" : "aktivieren"}
              </button>
              <button
                onClick={() => {
                  store?.setLeds([...new Array(256).fill(0)]);
                  wsMessage('clear');
                  store?.toast('Canvas gelöscht', 1000);
                }}
                style={{
                  "background": "#dc2626",
                  "color": "#fff",
                  "border": "none",
                  "padding": "8px 14px",
                  "border-radius": "6px",
                  "cursor": "pointer",
                  "font-size": "0.85rem"
                }}
              >
                🗑️ Alles löschen
              </button>
              <button
                onClick={() => {
                  wsMessage('persist');
                  store?.toast('Zeichnung im Flash gespeichert', 1500);
                }}
                style={{
                  "background": "#059669",
                  "color": "#fff",
                  "border": "none",
                  "padding": "8px 14px",
                  "border-radius": "6px",
                  "cursor": "pointer",
                  "font-size": "0.85rem"
                }}
              >
                💾 Speichern
              </button>
              <button
                onClick={() => {
                  wsMessage('load');
                  store?.toast('Zeichnung geladen', 1500);
                }}
                style={{
                  "background": "#4b5563",
                  "color": "#fff",
                  "border": "none",
                  "padding": "8px 14px",
                  "border-radius": "6px",
                  "cursor": "pointer",
                  "font-size": "0.85rem"
                }}
              >
                📂 Laden
              </button>
            </div>

            <LedMatrix
              disabled={false}
              data={store?.leds() || []}
              indexData={rotatedMatrix()}
              onSetLed={(data) => wsMessage('led', data)}
              onSetMatrix={(data) => store?.setLeds([...data])}
            />
          </div>
        </Show>

        {/* TAB 3: ESP32 Sensoren & Diagnostik */}
        <Show when={activeTab() === 'sensors'}>
          <div>
            <div style={{ "margin-bottom": "14px", "display": "flex", "align-items": "center", "justify-content": "space-between", "flex-wrap": "wrap", "gap": "10px" }}>
              <h2 style={{ "font-size": "1.1rem", "font-weight": "700", "margin": "0" }}>
                ESP32 Hardware-Sensoren & Systemmetriken
              </h2>
              <div style={{ "display": "flex", "gap": "8px", "align-items": "center" }}>
                <button
                  onClick={() => fetch('/api/plugin?name=Sensors%20Overview').then(() => store?.toast('🎛️ Kombi-Sensor-Dashboard auf Matrix aktiviert', 1500))}
                  style={{
                    "background": "#2563eb",
                    "color": "#fff",
                    "border": "none",
                    "padding": "6px 14px",
                    "border-radius": "6px",
                    "cursor": "pointer",
                    "font-size": "0.8rem",
                    "font-weight": "700",
                    "display": "inline-flex",
                    "align-items": "center",
                    "gap": "6px"
                  }}
                >
                  🎛️ Kombi-Dashboard auf Matrix
                </button>
                <span style={{ "font-size": "0.75rem", "color": "#10b981", "background": "rgba(16,185,129,0.1)", "padding": "4px 8px", "border-radius": "4px" }}>
                  Live
                </span>
              </div>
            </div>

            <div style={{
              "display": "grid",
              "grid-template-columns": "repeat(auto-fill, minmax(240px, 1fr))",
              "gap": "12px"
            }}>
              {/* Chip Temperature */}
              <div style={{
                "background": "#1e293b",
                "border": "1px solid #334155",
                "border-radius": "10px",
                "padding": "16px",
                "display": "flex",
                "flex-direction": "column"
              }}>
                <div style={{ "display": "flex", "align-items": "center", "justify-content": "space-between", "margin-bottom": "8px" }}>
                  <span style={{ "font-size": "0.85rem", "color": "#94a3b8" }}>Chip Temperatur</span>
                  <span style={{ "font-size": "1.3rem" }}>🌡️</span>
                </div>
                <div style={{ "font-size": "1.7rem", "font-weight": "800", "color": "#f97316" }}>
                  {store?.telemetry()?.temp_c !== undefined ? `${store?.telemetry()?.temp_c} °C` : '-- °C'}
                </div>
                <div style={{ "font-size": "0.75rem", "color": "#64748b", "margin-top": "4px", "margin-bottom": "12px" }}>
                  Interner On-Chip-Temperatursensor
                </div>
                <button
                  onClick={() => fetch('/api/plugin?name=Chip%20Temp').then(() => store?.toast('🌡️ Thermometer auf Matrix aktiviert', 1500))}
                  style={{
                    "margin-top": "auto",
                    "background": "#334155",
                    "color": "#f97316",
                    "border": "1px solid #f97316",
                    "padding": "6px 10px",
                    "border-radius": "6px",
                    "cursor": "pointer",
                    "font-size": "0.75rem",
                    "font-weight": "600"
                  }}
                >
                  👉 Thermometer auf Matrix
                </button>
              </div>

              {/* Hall Sensor */}
              <div style={{
                "background": "#1e293b",
                "border": "1px solid #334155",
                "border-radius": "10px",
                "padding": "16px",
                "display": "flex",
                "flex-direction": "column"
              }}>
                <div style={{ "display": "flex", "align-items": "center", "justify-content": "space-between", "margin-bottom": "8px" }}>
                  <span style={{ "font-size": "0.85rem", "color": "#94a3b8" }}>Hall Sensor (Magnet)</span>
                  <span style={{ "font-size": "1.3rem" }}>🧲</span>
                </div>
                <div style={{ "font-size": "1.7rem", "font-weight": "800", "color": "#818cf8" }}>
                  {store?.telemetry()?.hall !== undefined ? store?.telemetry()?.hall : '--'}
                </div>
                <div style={{ "font-size": "0.75rem", "color": "#64748b", "margin-top": "4px", "margin-bottom": "12px" }}>
                  Magnetfeld-Sensor (reagiert auf Magnete)
                </div>
                <button
                  onClick={() => fetch('/api/plugin?name=Hall%20Sensor').then(() => store?.toast('🧲 Hall-Visualizer auf Matrix aktiviert', 1500))}
                  style={{
                    "margin-top": "auto",
                    "background": "#334155",
                    "color": "#818cf8",
                    "border": "1px solid #818cf8",
                    "padding": "6px 10px",
                    "border-radius": "6px",
                    "cursor": "pointer",
                    "font-size": "0.75rem",
                    "font-weight": "600"
                  }}
                >
                  👉 Magnet-Visualizer auf Matrix
                </button>
              </div>

              {/* WiFi RSSI */}
              <div style={{
                "background": "#1e293b",
                "border": "1px solid #334155",
                "border-radius": "10px",
                "padding": "16px",
                "display": "flex",
                "flex-direction": "column"
              }}>
                <div style={{ "display": "flex", "align-items": "center", "justify-content": "space-between", "margin-bottom": "8px" }}>
                  <span style={{ "font-size": "0.85rem", "color": "#94a3b8" }}>WLAN Empfang (RSSI)</span>
                  <span style={{ "font-size": "1.3rem" }}>📶</span>
                </div>
                <div style={{ "font-size": "1.7rem", "font-weight": "800", "color": "#34d399" }}>
                  {store?.telemetry()?.rssi ? `${store?.telemetry()?.rssi} dBm` : '--'}
                </div>
                <div style={{ "font-size": "0.75rem", "color": "#64748b", "margin-top": "4px", "margin-bottom": "12px" }}>
                  Signalqualität: {store?.telemetry()?.rssi ? `${Math.min(100, Math.max(0, 2 * ((store?.telemetry()?.rssi || -100) + 100)))}%` : '--'}
                </div>
                <button
                  onClick={() => fetch('/api/plugin?name=WiFi%20Signal').then(() => store?.toast('📶 WiFi-Signal auf Matrix aktiviert', 1500))}
                  style={{
                    "margin-top": "auto",
                    "background": "#334155",
                    "color": "#34d399",
                    "border": "1px solid #34d399",
                    "padding": "6px 10px",
                    "border-radius": "6px",
                    "cursor": "pointer",
                    "font-size": "0.75rem",
                    "font-weight": "600"
                  }}
                >
                  👉 WiFi-Signal auf Matrix
                </button>
              </div>

              {/* RAM / Heap */}
              <div style={{
                "background": "#1e293b",
                "border": "1px solid #334155",
                "border-radius": "10px",
                "padding": "16px"
              }}>
                <div style={{ "display": "flex", "align-items": "center", "justify-content": "space-between", "margin-bottom": "8px" }}>
                  <span style={{ "font-size": "0.85rem", "color": "#94a3b8" }}>Freier RAM (Heap)</span>
                  <span style={{ "font-size": "1.3rem" }}>🧠</span>
                </div>
                <div style={{ "font-size": "1.5rem", "font-weight": "800", "color": "#38bdf8" }}>
                  {store?.telemetry()?.free_heap ? `${Math.round((store?.telemetry()?.free_heap || 0) / 1024)} KB` : '--'}
                </div>
                <div style={{ "font-size": "0.75rem", "color": "#64748b", "margin-top": "4px" }}>
                  Gesamt: {store?.telemetry()?.total_heap ? `${Math.round((store?.telemetry()?.total_heap || 0) / 1024)} KB` : '320 KB'}
                </div>
              </div>

              {/* CPU Frequency */}
              <div style={{
                "background": "#1e293b",
                "border": "1px solid #334155",
                "border-radius": "10px",
                "padding": "16px"
              }}>
                <div style={{ "display": "flex", "align-items": "center", "justify-content": "space-between", "margin-bottom": "8px" }}>
                  <span style={{ "font-size": "0.85rem", "color": "#94a3b8" }}>CPU Takt</span>
                  <span style={{ "font-size": "1.3rem" }}>⚡</span>
                </div>
                <div style={{ "font-size": "1.7rem", "font-weight": "800", "color": "#fbbf24" }}>
                  {store?.telemetry()?.cpu_mhz ? `${store?.telemetry()?.cpu_mhz} MHz` : '240 MHz'}
                </div>
                <div style={{ "font-size": "0.75rem", "color": "#64748b", "margin-top": "4px" }}>
                  Xtensa Dual-Core 32-bit LX6
                </div>
              </div>

              {/* System Uptime */}
              <div style={{
                "background": "#1e293b",
                "border": "1px solid #334155",
                "border-radius": "10px",
                "padding": "16px"
              }}>
                <div style={{ "display": "flex", "align-items": "center", "justify-content": "space-between", "margin-bottom": "8px" }}>
                  <span style={{ "font-size": "0.85rem", "color": "#94a3b8" }}>System Laufzeit</span>
                  <span style={{ "font-size": "1.3rem" }}>⏱️</span>
                </div>
                <div style={{ "font-size": "1.5rem", "font-weight": "800", "color": "#a78bfa" }}>
                  {formatUptime(store?.telemetry()?.uptime)}
                </div>
                <div style={{ "font-size": "0.75rem", "color": "#64748b", "margin-top": "4px" }}>
                  Unterbrechungsfreier Betrieb
                </div>
              </div>
            </div>

            {/* Network Info Footer */}
            <div style={{
              "background": "#0f172a",
              "border": "1px solid #1e293b",
              "border-radius": "8px",
              "padding": "14px",
              "margin-top": "16px",
              "display": "flex",
              "justify-content": "space-between",
              "flex-wrap": "wrap",
              "gap": "10px",
              "font-size": "0.8rem",
              "color": "#94a3b8"
            }}>
              <div>IP-Adresse: <b style={{ "color": "#f1f5f9" }}>{store?.telemetry()?.ip || '192.168.1.28'}</b></div>
              <div>MAC: <b style={{ "color": "#f1f5f9" }}>{store?.telemetry()?.mac || '08:B6:1F:37:DB:B4'}</b></div>
              <div>REST API: <a href="/api/sensors" target="_blank" style={{ "color": "#38bdf8" }}>/api/sensors (JSON)</a></div>
            </div>
          </div>
        </Show>

        {/* TAB 4: Lauftext Ticker */}
        <Show when={activeTab() === 'ticker'}>
          <div style={{
            "background": "#1e293b",
            "border": "1px solid #334155",
            "border-radius": "12px",
            "padding": "20px"
          }}>
            <h2 style={{ "font-size": "1.1rem", "font-weight": "700", "margin": "0 0 12px 0" }}>
              Lauftext auf der Matrix anzeigen
            </h2>
            <p style={{ "font-size": "0.85rem", "color": "#94a3b8", "margin-bottom": "16px" }}>
              Sende eine Nachricht direkt an die 16×16 Matrix. Die Nachricht scrollt flüssig über das Display.
            </p>

            <div style={{ "margin-bottom": "14px" }}>
              <label style={{ "display": "block", "font-size": "0.8rem", "margin-bottom": "6px", "color": "#cbd5e1" }}>
                Nachricht:
              </label>
              <input
                type="text"
                placeholder="z.B. Willkommen! oder Kaffee ist fertig"
                value={tickerText()}
                onInput={(e) => setTickerText(e.currentTarget.value)}
                style={{
                  "width": "100%",
                  "padding": "10px 14px",
                  "background": "#0f172a",
                  "border": "1px solid #334155",
                  "border-radius": "8px",
                  "color": "#fff",
                  "font-size": "0.95rem"
                }}
              />
            </div>

            <div style={{ "display": "flex", "gap": "14px", "margin-bottom": "18px", "flex-wrap": "wrap" }}>
              <div>
                <label style={{ "display": "block", "font-size": "0.8rem", "margin-bottom": "4px", "color": "#cbd5e1" }}>
                  Wiederholungen:
                </label>
                <select
                  value={tickerRepeat()}
                  onChange={(e) => setTickerRepeat(parseInt(e.currentTarget.value))}
                  style={{
                    "padding": "8px 12px",
                    "background": "#0f172a",
                    "border": "1px solid #334155",
                    "border-radius": "6px",
                    "color": "#fff"
                  }}
                >
                  <option value={1}>1x abspielen</option>
                  <option value={3}>3x abspielen</option>
                  <option value={5}>5x abspielen</option>
                  <option value={-1}>Dauerhaft wiederholen</option>
                </select>
              </div>

              <div>
                <label style={{ "display": "block", "font-size": "0.8rem", "margin-bottom": "4px", "color": "#cbd5e1" }}>
                  Geschwindigkeit ({tickerSpeed()}ms):
                </label>
                <input
                  type="range"
                  min="15"
                  max="80"
                  value={tickerSpeed()}
                  onInput={(e) => setTickerSpeedSignal(parseInt(e.currentTarget.value))}
                  style={{ "accent-color": "#38bdf8", "cursor": "pointer" }}
                />
              </div>
            </div>

            <div style={{ "display": "flex", "gap": "10px", "flex-wrap": "wrap", "margin-bottom": "24px" }}>
              <button
                onClick={sendTicker}
                style={{
                  "background": "#2563eb",
                  "color": "#fff",
                  "border": "none",
                  "padding": "10px 20px",
                  "border-radius": "8px",
                  "font-weight": "700",
                  "font-size": "0.95rem",
                  "cursor": "pointer",
                  "display": "inline-flex",
                  "align-items": "center",
                  "gap": "8px"
                }}
              >
                🚀 Nachricht an Matrix senden
              </button>
              <button
                onClick={stopTicker}
                style={{
                  "background": "#dc2626",
                  "color": "#fff",
                  "border": "none",
                  "padding": "10px 18px",
                  "border-radius": "8px",
                  "font-weight": "700",
                  "font-size": "0.95rem",
                  "cursor": "pointer",
                  "display": "inline-flex",
                  "align-items": "center",
                  "gap": "8px"
                }}
              >
                ⏹️ Ticker stoppen & Zurück zur Uhr
              </button>
            </div>

            {/* Quick Demos & Cluster Integration */}
            <div style={{
              "border-top": "1px solid #334155",
              "padding-top": "16px",
              "margin-top": "12px"
            }}>
              <h3 style={{ "font-size": "0.95rem", "font-weight": "700", "margin": "0 0 10px 0", "color": "#cbd5e1" }}>
                Direkte Ansichten & Schnittstellen testen:
              </h3>
              <div style={{ "display": "flex", "gap": "10px", "flex-wrap": "wrap" }}>
                <button
                  onClick={sendTransitDemo}
                  style={{
                    "background": "#1e293b",
                    "color": "#38bdf8",
                    "border": "1px solid #0284c7",
                    "padding": "8px 14px",
                    "border-radius": "6px",
                    "cursor": "pointer",
                    "font-size": "0.85rem",
                    "font-weight": "600"
                  }}
                >
                  🚆 ÖPNV: U1 (3m) aktivieren
                </button>
                <button
                  onClick={sendClusterDemo}
                  style={{
                    "background": "#1e293b",
                    "color": "#4ade80",
                    "border": "1px solid #16a34a",
                    "padding": "8px 14px",
                    "border-radius": "6px",
                    "cursor": "pointer",
                    "font-size": "0.85rem",
                    "font-weight": "600"
                  }}
                >
                  🖥️ Server-Cluster Monitor (5 Nodes)
                </button>
                <button
                  onClick={sendAlertDemo}
                  style={{
                    "background": "#1e293b",
                    "color": "#f87171",
                    "border": "1px solid #dc2626",
                    "padding": "8px 14px",
                    "border-radius": "6px",
                    "cursor": "pointer",
                    "font-size": "0.85rem",
                    "font-weight": "600"
                  }}
                >
                  🚨 Sentinel Alarm auslösen
                </button>
                <button
                  onClick={() => fetch('/api/river?cm=142&trend=1&activate=1').then(() => store?.toast('🌊 Fluss-Pegelstand aktiviert (142cm ▲)', 2000))}
                  style={{
                    "background": "#1e293b",
                    "color": "#38bdf8",
                    "border": "1px solid #0284c7",
                    "padding": "8px 14px",
                    "border-radius": "6px",
                    "cursor": "pointer",
                    "font-size": "0.85rem",
                    "font-weight": "600"
                  }}
                >
                  🌊 Fluss-Pegel (142cm ▲)
                </button>
                <button
                  onClick={() => fetch('/api/air?aqi=28&activate=1').then(() => store?.toast('🫁 Luftqualität aktiviert (AQI 28)', 2000))}
                  style={{
                    "background": "#1e293b",
                    "color": "#a78bfa",
                    "border": "1px solid #7c3aed",
                    "padding": "8px 14px",
                    "border-radius": "6px",
                    "cursor": "pointer",
                    "font-size": "0.85rem",
                    "font-weight": "600"
                  }}
                >
                  🫁 Luftqualität (AQI 28)
                </button>
                <button
                  onClick={() => fetch('/api/rain?min=12&intensity=65&activate=1').then(() => store?.toast('🌧️ Regenradar aktiviert (in 12 Min)', 2000))}
                  style={{
                    "background": "#1e293b",
                    "color": "#60a5fa",
                    "border": "1px solid #2563eb",
                    "padding": "8px 14px",
                    "border-radius": "6px",
                    "cursor": "pointer",
                    "font-size": "0.85rem",
                    "font-weight": "600"
                  }}
                >
                  🌧️ Regenradar (in 12m)
                </button>
                <button
                  onClick={() => fetch('/api/energy?percent=82&activate=1').then(() => store?.toast('⚡ Grünstrom-Index aktiviert (82%)', 2000))}
                  style={{
                    "background": "#1e293b",
                    "color": "#fbbf24",
                    "border": "1px solid #d97706",
                    "padding": "8px 14px",
                    "border-radius": "6px",
                    "cursor": "pointer",
                    "font-size": "0.85rem",
                    "font-weight": "600"
                  }}
                >
                  ⚡ Grünstrom (82%)
                </button>
                <button
                  onClick={() => fetch('/api/iss?min=3&visible=1&activate=1').then(() => store?.toast('🛰️ ISS Tracker aktiviert (Überflug in 3m)', 2000))}
                  style={{
                    "background": "#1e293b",
                    "color": "#f472b6",
                    "border": "1px solid #db2777",
                    "padding": "8px 14px",
                    "border-radius": "6px",
                    "cursor": "pointer",
                    "font-size": "0.85rem",
                    "font-weight": "600"
                  }}
                >
                  🛰️ ISS Tracker (3m)
                </button>
              </div>
            </div>
          </div>
        </Show>

      </main>
    </div>
  );
};

function createSpeedSignal(initial: number) {
  return createSignal(initial);
}

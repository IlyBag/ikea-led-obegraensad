import { Setter } from 'solid-js';

export enum SYSTEM_STATUS {
  NONE = 'draw',
  WSBINARY = 'wsbinary',
  // SYSTEM
  UPDATE = 'update',
  LOADING = 'loading',
}

export interface Telemetry {
  temp_c: number;
  temp_f: number;
  hall: number;
  cpu_mhz: number;
  free_heap: number;
  total_heap: number;
  flash_size: number;
  sketch_size: number;
  free_sketch: number;
  rssi: number;
  uptime: number;
  ip: string;
  mac: string;
  brightness: number;
  active_plugin: number;
  active_name: string;
}

export interface Store {
  rotation: () => number;
  brightness: () => number;
  indexMatrix: () => number[];
  leds: () => number[];
  plugins: () => { id: number; name: string }[];
  plugin: () => number;
  systemStatus: () => SYSTEM_STATUS;
  telemetry: () => Telemetry | null;

  setRotation: Setter<number>;
  setBrightness: Setter<number>;
  setIndexMatrix: Setter<number[]>;
  setLeds: Setter<number[]>;
  setPlugin: Setter<number>;
  setSystemStatus: Setter<SYSTEM_STATUS>;
  setTelemetry: Setter<Telemetry | null>;
  send: (message: string | ArrayBuffer) => void;
  connectionState: () => number;

  connectionStatus?: string;
  toast: (text: string, duration: number) => void;
}

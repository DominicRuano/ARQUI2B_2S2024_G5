import { defineConfig } from 'vite'
import react from '@vitejs/plugin-react'

// https://vitejs.dev/config/
export default defineConfig({
  plugins: [react()],
  server: {
    host: true, // Esto permitirá que Vite escuche en la red local
    port: 3000, // El puerto en el que quieres que corra tu aplicación
  }
})

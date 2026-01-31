import path from 'path';
import tailwindcss from '@tailwindcss/vite';
import react from '@vitejs/plugin-react';
import viteCompression from 'vite-plugin-compression';
import { defineConfig } from 'vite';

// https://vite.dev/config/
export default defineConfig({
  plugins: [
    react(),
    tailwindcss(),
    viteCompression({
      algorithm: 'gzip',
      ext: '.gz',
      threshold: 124,
      deleteOriginFile: false,
      filter: /\.(html|css|js|ico)$/,
    }),
  ],
  server: {
    proxy: {
      '/api': {
        target: 'http://192.168.4.1',
        changeOrigin: true,
      }
    }
  },
  build: {
    minify: 'esbuild',
    target: 'esnext',
    sourcemap: false,
    outDir: 'dist',
    rollupOptions: {
      output: {
        entryFileNames: 'main.js',
        chunkFileNames: 'chunk.js',
        assetFileNames: 'main.[ext]',
        compact: true,
      },
    },
  },
  resolve: {
    alias: {
      '@': path.resolve(__dirname, './src'),
    },
  },
  optimizeDeps: {
    include: ['react', 'react-dom'], // Предварительная оптимизация React
    esbuildOptions: {
      minify: true,
    },
  },
  define: {
    'process.env.NODE_ENV': '"production"',
  },
});

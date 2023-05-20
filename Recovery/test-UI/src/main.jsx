import React from 'react'
import ReactDOM from 'react-dom/client'
import App from './App.jsx'
import Settings from './settings.jsx'
import './index.css'

ReactDOM.createRoot(document.getElementById('root')).render(
  <React.StrictMode>
    <Settings />
    <App />
  </React.StrictMode>,
)

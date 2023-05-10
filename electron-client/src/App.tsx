import { useState } from 'react'
import Router from './components/router/router'

console.log('[App.tsx]', `Hello world from Electron ${process.versions.electron}!`)

function App() {
  const [count, setCount] = useState(0)
  return (
   <Router/>
  )
}

export default App

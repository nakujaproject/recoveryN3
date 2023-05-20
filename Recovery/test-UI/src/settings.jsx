import { useState } from "react"
export default function Settings() {
  let [option, setOption] = useState('broker');

  return (
    <div className="overlay" id="settings">
      <div className="relative container mx-auto mt-5 rounded-lg h-vh bg-slate-100">
        <h1>Settings</h1>
        <button className="absolute right-0 text-xl" onClick={e=>{document.getElementById('settings').style.visibility='hidden'}}>X</button>
        <div className="flex justify-around">
          <div className="flex flex-col">
            <label>Broker Address</label>
            <input type="text" name="broker" id="broker" />
          </div>
        </div>
      </div>
    </div>
  )
}
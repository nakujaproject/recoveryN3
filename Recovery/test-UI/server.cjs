const express = require('express');
const path = require('path');
const mime = require('mime');

const app = express();
const port = 8081;

// sendFile will go here
app.use(express.static(path.join(__dirname, 'dist')));

app.listen(port,()=>{
    console.log('Server started at http://localhost:' + port);
});
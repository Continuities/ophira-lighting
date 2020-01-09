const express = require('express');
const app = express();

app.use(express.static('build', {
  index: 'harness.html'
}));

app.listen(3000, () => {});
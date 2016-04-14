var express = require('express');
var router = express.Router();

/* GET users listing. */
router.get('/', function(req, res, next) {
  console.log(next);
  console.log("blah");
  res.render('index', { title: 'Users' });

});

router.get('/register',function (req,res) {
  
})
module.exports = router;

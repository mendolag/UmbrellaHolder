var express = require('express');
var router = express.Router();

/* GET users listing. */
/* GET home page. */
router.get('/', function(req, res, next) {
  res.render('index',{title:"Umbrella holder"});
});

router.get('/register',function (req, res) {
  res.render('register');
});


router.post('/register',function(req,res){

});


router.get('/', function(req, res, next) {
  console.log(next);
  console.log("blah");
  res.render('index', { title: 'Users' });

});


router.get('/user/:username',function (req,res) {
  
});

router.get('/user/reminders',function(req,res){
  
});


module.exports = router;

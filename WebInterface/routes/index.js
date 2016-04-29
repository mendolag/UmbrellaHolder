var express = require('express');
var router = express.Router();
var hf=require('../forcast/funcs');

/* GET home page. */
router.get('/', function(req, res, next) {
  res.render('index',{title:"Umbrella holder"});
});

router.get('/register',function (req, res) {
  res.render('register');
});


router.post('/register',function(req,res){

});


router.get('/todayForecast',function(req,res){
  hf.forcastRequest(function(err,req,body){
    if(err){
      console.log("error: "+err);
    }else{
      console.log("Without Error")
      console.log(req);
      console.log("//////////////////");
      console.log(body);
      res.send(body);

    }
  })
});


module.exports = router;

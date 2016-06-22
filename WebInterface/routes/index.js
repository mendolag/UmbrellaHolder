var dbFuncs=require('../database/dbFuncs');
var hf=require('../forcast/funcs');
var moment=require('moment');
/* GET users listing. */
/* GET home page. */
module.exports=function(app,passport) {


  app.get('/', function (req, res) {
    res.render('index', {title: "Umbrella holder"});
  });

  // router.get('/register',function (req, res) {
  //   res.render('register');
  //
  // });
  //
  //
  // router.post('/register1',function(req,res){
  //   console.log("1")
  //   var body=req.body;
  //   console.log(body)
  //   console.log(req.params)
  //   //router.use(express.cookieParser());
  //   console.log("2")
  //
  //   express.use(express.session({secret: '1234567890QWERTY'}));
  //   console.log("3")
  //
  //   res.cookie('username',body.name );
  //   console.log("4")
  //   console.log(req.cookie);
  //
  //   });
  //
  //
  // router.get('/register2',function(req,res){
  //
  //   console.log('redirected!!!!');
  //   var body=req.body;
  //   console.log(req.cookie);
  //   res.render('register2');
  // });


  app.get('/login', function (req, res) {
    //console.log(req.params)
    res.render('login.ejs', {message: req.flash('loginMessage')});
  });
  //Signup page
  app.get('/signup', function (req, res) {
    res.render('signup.ejs', {message: req.flash('signupMessage')});
  });

  //Profile
  app.get('/profile',isLoggedIn,function (req, res) {
    console.log(req.user);
    res.render('profile.ejs',{
      user:req.user, smessage:undefined, fmessage:undefined
    });
  });

  app.get('/logout', function (req,res) {
    req.logout();
    res.redirect('/');
  });

  app.get("/registerBl",isBleRegistered ,function(req,res){
    console.log(req.user)
    res.render('bleRegister.ejs',{user:req.user});
  });


  app.get("/assignBle/:id",isBleRegistered,function (req,res) {
    var user=req.user;
    var blid=req.params.id
    dbFuncs.assignDeviceToUser(user._id,blid,function(err,message){
      if(err){
        console.log(err);
        res.render("profile.ejs",{user:user,smessage:undefined,fmessage:err});
      }else{

          user.bluetoothId=blid;
          res.render("profile.ejs",{user:user,smessage:message.message,fmessage:undefined});


      }

    })
  })
  //Process login form

  app.post('/login', passport.authenticate('local-login', {
    successRedirect: '/profile',
    failureRedirect: '/login',
    failureFlash: true
  }));




  //Process to signup
  app.post('/signup', passport.authenticate('local-signup', {
    successRedirect: '/profile',
    failureRedirect: '/signup',
    failureFlash: true
  }));

  //
  // app.get('/profile', isLoggedIn, function (req, res) {
  //   //console.log(req.user._id);
  //   req.session.touch();
  //   //console.log("SeSSion1:");
  //   //console.log( req.session)
  //   DBfunc.getFullUser(req.user._id, function (err, user) {
  //     if (err) {
  //       throw err
  //     }
  //     if (user) {
  //       //console.log(user.role);
  //
  //       res.render('profile.ejs', {
  //         user: user,
  //         role: user.role.name,
  //         message: req.flash('loginMessage') || req.flash('signupMessage')
  //       });
  //     } else {
  //       res.redirect("/");
  //     }
  //   });

  //passport.serializeUser(user);
  //DBfunc.getRole(user.role,function(err,r){
  //    if(err){
  //        throw err;
  //    }
  //    res.render('profile.ejs',{
  //        user : req.user,
  //        role: r.name,
  //        message: req.flash('loginMessage')||req.flash('signupMessage')
  //    });
  //})
  //);


  app.get('/getUnregisteredBluetooth', function (req, res) {
    console.log("getUnregistered");
    dbFuncs.getUnregisteredBluetooth(function (err, devices) {
      if (err) {
        throw err;
      } else if (devices) {
        res.send(devices);
      } else {
        res.send({devFound: false, message: 'make sure your bluetooth is registered'})
      }
    })
  });

  app.get('/', function (req, res, next) {

    res.render('index', {title: 'Users'});

  });


  app.get('/user/:username', function (req,res) {

  });

  app.get('/getReminders', function (req,res) {
    dbFuncs.getReminders(req.user.username,function (err,reminders) {
      if(err){
        res.send("somthing went wrong");
      }else if(reminders){
        console.log("reminder printing");
        console.log(reminders);
        res.send(reminders);
      }else{
        res.send("No reminders added yet");
      }
    })
  });

  app.get('/addReminder',isLoggedIn,function(req,res){
    res.render('addReminder');
  });

  app.post('/addReminder',isLoggedIn,function (req,res) {
    var rem=req.body;
    var user=req.user;
    var from=rem.from.split(",")
    var fromH=parseInt(from[0]);
    console.log(rem);
    if(from[1]=="pm"){
      fromH=fromH+12;
    }
    var to=rem.to.split(",")
    var toH=parseInt(to[0]);
    if(to[1]=="pm"){
      toH=toH+12;
    }
    var myDays=getDays(rem.days);
    var reminder={
      description:rem.description,
      condition:rem.condition,
      hour:{
        from:fromH,
        to:toH
      },
      days:myDays
    };
    console.log(reminder);
    dbFuncs.addReminder(user.username,reminder,function(err){
      if(err){
        console.log(err);
        throw err;
      }
    })
  });


  /*
   Arduino Api
   */

  app.get('/reminder/:blID', function (req, res) {
    var blId = req.params.blID;
    console.log(blId);
    dbFuncs.getUserByBlue(blId, function (err, user) {
      if (err) {
        console.log("error 1");
        var message = {message: "Database error", error: {status: res.statusCode, stack: err}}
        resError(req, res, message);
      } else if (user) {
        hf.forcastRequest(function (err, req, body) {
          if(err){
            console.log("error requesting forecast");
          }
          else{
            var currentReminders=[];
            dbFuncs.getReminders(user.username,function (err,reminders) {
              if(err){
                resError(req,res,err);
              }else{
                for(var i=0;i<reminders.length;i++){
                  var r=reminders[i];
                  var currentH=hf.getHour();
                  if(r.days[hf.getDay()]){
                    if(r.condition=='Always'|| r.condition==body.current.icon||r.condition==body.nextHour.icon||r.condition==body.today.icon){
                      if(parseInt(r.hour.from)<currentH && parseInt(r.hour.to)>currentH){
                        var reminder={
                          username:r.username,
                          hook:1,
                          description:r.description,
                          uuid:blId
                        }
                        currentReminders.push(reminder)
                      }
                    }
                  }
                }

              }

              res.send(currentReminders);
            })}
        })
        // console.log("getting user reminders");
        // console.log(body);

  } else {
    dbFuncs.registerBluetooth(blId, function (err, registered) {
      if (err) {

        console.log("error 2");
        var message = {message: "Bluetooth registration error", error: {status: res.statusCode, stack: err}}
        resError(req, res, message);
      } else if (!registered) {
        var message = {message: "Bluetooth ID exists", error: {status: res.statusCode, stack: err}}
        resError(req, res, message);
      } else if (registered) {
        res.send("New Bluetooth registered");
      }
    })
  }
})
});




app.get('/todayForecast', function (req, res) {
  hf.forcastRequest(function (err, req, body) {
    if (err) {
      console.log("error: " + err);
    } else {
      console.log(body);
      res.send(body);
      res.end();

    }
  })
});

function resError(req, res, error) {
  res.render('error', error);
}

};
function isBleRegistered(req,res,next){
  if(req.isAuthenticated()&&req.user.bluetoothId==null){
    return next();}
  res.render('/profile',{user:req.user,smessage:undefined,fmessage:'You have already registered a Bluetooth'});

}
function isLoggedIn(req, res, next) {

  // if user is authenticated in the session, carry on
  if (req.isAuthenticated())
    return next();

  // if they aren't redirect them to the home page
  res.redirect('/');
}


function getDays(days){
  var myDays={  mon:false, tue:false,
    wed:false, thu:false,
    fri:false, sat:false,
    sun:false}
  myDays.mon=(days.indexOf("mon") > -1);
  myDays.tue=(days.indexOf("tue") > -1);
  myDays.wed=(days.indexOf("wed") > -1);
  myDays.thu=(days.indexOf("thu") > -1);
  myDays.fri=(days.indexOf("fri") > -1);
  myDays.sat=(days.indexOf("sat") > -1);
  myDays.sun=(days.indexOf("sun") > -1);
  return myDays;
}










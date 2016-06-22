var express = require('express');
var app = express();

var mongoose =require('mongoose');
var passport = require('passport');
var flash    = require('connect-flash');

var morgan=require("morgan");
var cookieParser = require('cookie-parser');
var bodyParser = require('body-parser');
var session =require('express-session');
var configDB=require('./database/config');


var favicon = require('serve-favicon');




var path     = require('path');







mongoose.connect(configDB.url);
require('./sessionManager/passport')(passport);
app.use(morgan('dev'));
app.use(cookieParser());
app.use(bodyParser());

// view engine setup

app.set('views', path.join(__dirname, 'views'));
app.set('view engine', 'ejs');
app.use(session({secret:'umbrellaHolder'}))

// session secret
app.use(passport.initialize());
app.use(passport.session()); 
app.use(flash());

/**
 * Session managment
 */
app.use(session({ secret: 'MyKEY', cookie : {
  expires: false,
  secure:false
}
}));
//routes ===================================================================================

//var arduino = require('./routes/index')(app);
require('./routes/index')(app,passport);
//var setup = require('./routes/setup');


//app.use('/arduino', arduino);
//app.use('/', users);
//app.use('/admin',setup);


// uncomment after placing your favicon in /public
//app.use(favicon(path.join(__dirname, 'public', 'favicon.ico')));

app.use(bodyParser.urlencoded({ extended: false }));

app.use(express.static(path.join(__dirname, 'public')));
app.use('/bower_components', express.static(path.join(process.cwd(), 'bower_components')));
app.use('/components', express.static(path.join(process.cwd(), 'components')));
app.use('/style',express.static(path.join(process.cwd(), 'views/style')));





// catch 404 and forward to error handler
app.use(function(req, res, next) {
  var err = new Error('Not Found');
  err.status = 404;
  next(err);
});

// error handlers

// development error handler
// will print stacktrace
if (app.get('env') === 'development') {
  app.use(function(err, req, res, next) {
    res.status(err.status || 500);
    res.render('error', {
      message: err.message,
      error: err
    });
  });
}

// production error handler
// no stacktraces leaked to user
app.use(function(err, req, res, next) {
  res.status(err.status || 500);
  res.render('error', {
    message: err.message,
    error: {}
  });
});




module.exports = app;

var express  = require('express');
var app      = express();

var net = require('net');
var ns = net.createServer();

var server = require('http').Server(app);
var io = require('socket.io')(server);

var mongoose = require('mongoose');
var passport = require('passport');
var flash    = require('connect-flash');

var morgan       = require('morgan');
var cookieParser = require('cookie-parser');
var bodyParser   = require('body-parser');
var session      = require('express-session');

var info =[];

// mongoose.connect('mongodb://localhost/login');
mongoose.connect('mongodb://localhost/login', { useNewUrlParser: true });

require('./config/passport')(passport);

// set up our express application
app.use(morgan('dev'));

//read cookies
app.use(cookieParser());

//Body parser middleware
app.use(bodyParser());

//Set static folder
app.use(express.static(__dirname + '/public'));

//View engine
app.set('view engine', 'ejs'); 

//Express session middleware
app.use(session({ secret: 'topsecret' })); // session secret

//Passport init
app.use(passport.initialize());
app.use(passport.session()); 

//Connect flash middleware
app.use(flash());  

//Routes
require('./config/netserver.js')(ns,io,info);
require('./app/routes.js')(app, passport,io,info); // load our routes and pass in our app and fully configured passport

//Server
ns.listen(8081,"0.0.0.0" );
server.listen(3000,'0.0.0.0');
console.log('Server chay tren port 3000');
console.log('Net server chay tren port 8081')
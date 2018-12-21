var User            = require('../app/models/user');
var clientsio = {}
module.exports = function(app, passport,io,info) {

    //
    app.use(function (req, res, next) {
      console.log(req.body);
      
    next()
    });
    //toi mapping2 neu tai khoan da duoc chon duoc board va doi thu
     app.get('/mapping2',isLoggedIn,function(req,res){
       var index = findIndexUser(info,req.user.local.email);
       console.log(info);
       console.log(index);
       if(index!=-1){
        //chon board va doi thu thanh cong se chuyen toi mapping2 de choi
        res.render('mapping2.ejs',{ useraut:req.user.local.email,indexaut:index});  
       }
       else
       //tra lai list board neu khong chon duoc board va doi thu
         res.redirect('/listboard');
     });
     //
     io.of('/mapping2').on('connect', function(socket){
       //Hien thi info cua board khi ket noi den server
       console.log('Co nguoi ket noi den netserver ' + socket.id);
       var temp1;
       var countdown =60;
       
       socket.on('get user',function(temp){
         var index = findIndexUser(info,temp);
         if(index==-1)
           return 0;
         User.findOne({ 'local.email' :  temp }, function(err, user) {
         
           var enemyId = user.enemyId;
            var map_ta = new Array(8);
            for (var i = 0; i < map_ta.length; i++) {
              map_ta[i] = new Array(8);
            }
            for (var i = 0;i<64;i++)
            {
              if(user.locateboat[i].isBoat)
                map_ta[user.locateboat[i].x][user.locateboat[i].y] = "A";
              if(user.locateboat[i].isShoot)
                map_ta[user.locateboat[i].x][user.locateboat[i].y] = "S";
              if(user.locateboat[i].isBreak)
                map_ta[user.locateboat[i].x][user.locateboat[i].y] = "B";
            }
            
            var map_dich = new Array(8);
            for (var i = 0; i < map_dich.length; i++) {
              map_dich[i] = new Array(8);
            }
            User.findOne({ '_id' :  enemyId }, function(err, usere) {
              for (var i = 0;i<64;i++)
              {
              
                if(usere.locateboat[i].isShoot)
                  map_dich[usere.locateboat[i].x][usere.locateboat[i].y] = "S";
                if(usere.locateboat[i].isBreak)
                  map_dich[usere.locateboat[i].x][usere.locateboat[i].y] = "B";
              }
             
             
             
             socket.emit('update tadich',map_ta,map_dich,info,index,user.isTurn,countdown);
             });
             
         });
         temp1 = temp;
       });
//set turn cho user va cho 
       socket.on('user fire',function(index){
           info[index].isFire=false;
           User.findOne({ 'local.email' :  temp1 }, function(err, user) {
             if(1)//user.isTurn
             {
             var enemyId = user.enemyId;
             User.findOneAndUpdate({'$and':[
                                  {'_id': enemyId},{'locateboat': {'$elemMatch':
                                  {'x':info[index].locatex,'y':info[index].locatey}}
            }]},
                              {'$set':{'locateboat.$.isShoot':true}},
                              function(err,maganer){
                              });
            User.findOneAndUpdate({'$and':[
                                  {'_id': enemyId},{'locateboat': {'$elemMatch':
                                  {'x':info[index].locatex,'y':info[index].locatey,'isBoat':true}}
            }]},
                              {'$set':{'locateboat.$.isBreak':true}},
                              function(err,maganer){
                              var map_ta = new Array(8);
                              for (var i = 0; i < map_ta.length; i++) {
                                map_ta[i] = new Array(8);
                              }
                              for (var i = 0;i<64;i++)
                              {
                                if(user.locateboat[i].isBoat)
                                  map_ta[user.locateboat[i].x][user.locateboat[i].y] = "A";
                                if(user.locateboat[i].isShoot)
                                  map_ta[user.locateboat[i].x][user.locateboat[i].y] = "S";
                                if(user.locateboat[i].isBreak)
                                  map_ta[user.locateboat[i].x][user.locateboat[i].y] = "B";
                              }
                              
                              var map_dich = new Array(8);
                              for (var i = 0; i < map_dich.length; i++) {
                                map_dich[i] = new Array(8);
                              }
                              
                              User.findOne({ '_id' :  enemyId }, function(err, usere) {
                                for (var i = 0;i<64;i++)
                                {
                                
                                  if(usere.locateboat[i].isShoot)
                                    map_dich[usere.locateboat[i].x][usere.locateboat[i].y] = "S";
                                  if(usere.locateboat[i].isBreak)
                                  {
                                    map_dich[usere.locateboat[i].x][usere.locateboat[i].y] = "B";
                                    
                                  }
                                  
                                }
                                
                                
                                if(!maganer)
                                {
            
                                  map_dich[info[index].locatex][info[index].locatey]="S";
                                }
                                else
                                {
                                  map_dich[info[index].locatex][info[index].locatey]="B";
                                  
                                }
                                
                               socket.emit('update tadich',map_ta,map_dich,info,index,false,countdown);                     
                              });
                                
                              });
           
            User.findByIdAndUpdate(enemyId,
                    {"$set":{ "isTurn": true}},
                    function (err, managerparent) {
                        if (err) throw err;
                               });
            User.findByIdAndUpdate(user._id,
                    {"$set":{ "isTurn": false}},
                    function (err, managerparent) {
                        if (err) throw err;
                               });
             
            }                     
                        
           });
         });
        socket.on('update countdown',function(temp){
          countdown = temp;
        });
       socket.on('disconnect', function(err){
         
         console.log('someone disconnect mapping2 from: ' + socket.id);
       });
     });
     //chuyen toi listboard khi co href la /listboard
    app.get('/listboard',isLoggedIn,function(req,res){
      setImmediate(function(){
        res.render('listboard.ejs',{ useraut:req.user.local.email});
      });
      
        
      
    });
    //update status user xong push len terminal
    io.of('/listboard').on('connect', function(socket){
      console.log('someone connected listboard from: ' + socket.id);
      socket.emit('update board', info);
      var tmp = '';
      socket.on('send user',function(data){
        tmp = data;
      });
      socket.on('select board',function(i){
          console.log(socket.id + ': chon board '+(i+1));
          info[i].status = "Connected";
          info[i].user=tmp;
          info[i].locatex=0;
          info[i].locatey=0;
          info[i].isFire=false;
          io.of('/listboard').emit('update board', info);
        });
      socket.on('disconnect board',function(i){
        console.log(socket.id + ': bo board '+(i+1));
        info[i].status = "waitting";
        info[i].user='';
        info[i].locatex=0;
        info[i].locatey=0;
        info[i].isFire=false;
        io.of('/listboard').emit('update board', info);
      });
      
    });
    //chuyen toi index khi co href la /
    app.get('/', function(req, res) {
        res.render('index.ejs'); // 
    });

    //chuyen toi dang nhap khi co href la /login
    app.get('/login', function(req, res) {
        res.render('login.ejs', { message: req.flash('loginMessage') }); 
    });
    //xu ly login
   app.post('/login', passport.authenticate('local-login', {
        successRedirect : '/profile',
        failureRedirect : '/login', 
        failureFlash : true
        
    }));

    //sign up
    app.get('/signup', function(req, res) {
        res.render('signup.ejs', { message: req.flash('signupMessage') });
    });

    //xu ly dang ky
   app.post('/signup', passport.authenticate('local-signup', {
        successRedirect : '/profile', //chuyen huong toi profile neu dang nhap thanh cong
        failureRedirect : '/signup', // quay lai vi tri ban dau neu fail
        failureFlash : true 
        
    }));

    // =====================================
    // Thông tin user dang ký =====================
    // =====================================
    app.get('/profile', isLoggedIn, function(req, res) {
        res.render('profile.ejs', {
            user : req.user // push info user len profile
        });
    });

    // =====================================
    // Ðang xu?t ==============================
    // =====================================
    app.get('/logout', function(req, res) {
        req.logout();
        res.redirect('/login');
    });
    app.get('/fire',isLoggedIn, function(req,res){
      res.render('fire.ejs',{message: req.flash('checkboat')});
      ;
    });
    app.post('/fire',isLoggedIn,function(req,res){
      var user = req.user;
      var enemyId = user.enemyId;
       User.findOneAndUpdate({'$and':[
                            {'local.email': user.local.email},{'locateEnemy': {'$elemMatch':
                            {'x':parseInt(req.body.x),'y':parseInt(req.body.y)}}
      }]},
                        {'$set':{'locateEnemy.$.isShoot':true}},
                        function(err,maganer){
                        });
      User.findOneAndUpdate({'$and':[
                            {'_id': enemyId},{'locateboat': {'$elemMatch':
                            {'x':parseInt(req.body.x),'y':parseInt(req.body.y)}}
      }]},
                        {'$set':{'locateboat.$.isbreak':true}},
                        function(err,maganer){
                        
                        if(!maganer)
                        {
                          if(req.body.json == "on")
                            res.send({mess:'Khong ban trung muc tieu'});
                          req.flash('checkboat','Muc tieu khong ton tai.');
                          
                        }
                        else
                        {
                          User.findOneAndUpdate({'$and':[
                            {'local.email': user.local.email},{'locateEnemy': {'$elemMatch':
                            {'x':parseInt(req.body.x),'y':parseInt(req.body.y)}}
                            }]},
                        {'$set':{'locateEnemy.$.isBoat':true}},
                        function(err,maganer){
                        });
                          if(req.body.json == "on")
                            res.send({mess:'Da ban trung muc tieu'});
                          req.flash('checkboat','Da ban trung muc tieu.');
                           socket.write("RUNG");
                        } 
                        console.log(req.user);
                        if(req.body.json != "on")
                          res.redirect('/fire');
                        });
    });
    app.get('/mapping',isLoggedIn,function(req,res){
      var user = req.user;
      var enemyId = user.enemyId;
      var map_ta = new Array(8);
      for (var i = 0; i < map_ta.length; i++) {
        map_ta[i] = new Array(8);
      }
      for (var i = 0;i<8;i++)
      {
        map_ta[user.locateboat[i].x][user.locateboat[i].y] = "A";
        if(user.locateboat[i].isbreak)
          map_ta[user.locateboat[i].x][user.locateboat[i].y] = "B";
      }
      
      var map_dich = new Array(8);
      for (var i = 0; i < map_dich.length; i++) {
        map_dich[i] = new Array(8);
      }
      
      for (var i = 0;i<8*8;i++)
      {
      
      if(user.locateEnemy[i].isShoot)
        map_dich[user.locateEnemy[i].x][user.locateEnemy[i].y] = "S";
      if(user.locateEnemy[i].isShoot & user.locateEnemy[i].isBoat)
        map_dich[user.locateEnemy[i].x][user.locateEnemy[i].y] = "B";
      }
      User.findOne({ '_id' :  enemyId }, function(err, maganer) {
      console.log(maganer);
      });
      console.log(user);
      
      res.render('mapping.ejs',{
        ta : map_ta, dich: map_dich
        });
    });
    
    app.get('/matching',isLoggedIn,function(req,res){
      res.render('matching.ejs',{message: req.flash('checkenemy')});
    });
    app.post('/matching',isLoggedIn,function(req,res){
      var user = req.user;
      User.findOne({ 'local.email' :  req.body.enemy }, function(err, manager){
          if(manager)
          {
            req.flash('checkenemy','Doi thu co ton tai.');
            user.enemyId = manager._id;
            user.save(function(err) {
                    if (err)
                        throw err;
                        });
            res.redirect('/refreshuser');
          }
          else
          {
            req.flash('checkenemy','Doi thu khong ton tai.');
            res.redirect('/matching');
          }
      });
    });
    app.get('/refreshenemy',isLoggedIn,function(req,res){
      var user = req.user;
      var enemyId = user.enemyId;
      User.findByIdAndUpdate(enemyId,
                  {"$set":{"locateboat":[]}},
                      function(err,manager){
                    
                    });
            for(var i = 0;i<8;i++){
               var jcheck = getRandomInt(8);
                for(var j = 0;j<8;j++){
                  if(jcheck==j){
                    var NewLocateBoat = makeNewLocateBoat(i,j,true,false,false);
                    }
                  else
                  {
                    var NewLocateBoat =  makeNewLocateBoat(i,j,false,false,false);
                  }
                  User.findByIdAndUpdate(enemyId,
                    {"$push":{ "locateboat": NewLocateBoat}},{ "new": true, "upsert": true},
                    function (err, managerparent) {
                        if (err) throw err;
                               });
                }
            }
            //set turn neu thua se ra ngoai
      User.findByIdAndUpdate(enemyId,
                    {"$set":{ "isTurn": false,"enemyId":user._id}},
                    function (err, managerparent) {
                        if (err) throw err;
                               });
            
      res.redirect('/matching');
    });
    //set profile user locate boat
    app.get('/refreshuser',isLoggedIn,function(req,res){
      var user = req.user;
      User.findByIdAndUpdate(user._id,
                  {"$set":{"locateboat":[]}},
                      function(err,manager){
                    
                    });
    //tao vi tri random thuyen trong map
            for(var i = 0;i<8;i++){
               var jcheck = getRandomInt(8);
                for(var j = 0;j<8;j++){
                  if(jcheck==j){
                    var NewLocateBoat = makeNewLocateBoat(i,j,true,false,false);
                    }
                  else
                  {
                    var NewLocateBoat =  makeNewLocateBoat(i,j,false,false,false);
                  }
                  User.findByIdAndUpdate(user._id,
                    {"$push":{ "locateboat": NewLocateBoat}},{ "new": true, "upsert": true},
                    function (err, managerparent) {
                        if (err) throw err;
                               });
                }
            }
            User.findByIdAndUpdate(user._id,
                    {"$set":{ "isTurn": true}},
                    function (err, managerparent) {
                        if (err) throw err;
                               });
      console.log(user);
      res.redirect('/refreshenemy');
    });
};

//Check loggin
function isLoggedIn(req, res, next) {
    if (req.isAuthenticated())
        return next();
    res.redirect('/');
}
//tao random
function getRandomInt(max) {
  return Math.floor(Math.random() * Math.floor(max));
}
//tao vi tri moi 
function makeNewLocateBoat(x1,y1,boatis,breakis,shootis)
{
  return {
    x: x1,
    y: y1,
    isBoat           :boatis,
    isBreak          :breakis,
    isShoot          :shootis
  };
}
//check user
function findIndexUser(info,user)
{
  for(var i = 0;i < info.length;i++){
    if(info[i].user == user)
      return i;
  }
  return -1;
}

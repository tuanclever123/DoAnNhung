var clients =[];

var temp = {};
module.exports = function(ns,io,info) {

  
  //tao status cho profile khi ket noi toi netserver io
  ns.on('connection', function connection(socket) {
  socket.name = socket.remoteAddress + ":" + socket.remotePort 
  clients.push(socket);
  temp.name = socket.name;
  temp.status = "waitting";
  temp.user="";
  temp.locatex=0;
  temp.locatey=0;
  temp.isFire = false;
  info.push(temp);

  temp ={};
  socket.write("Welcome " + socket.name + "\n");//Up len hien thi board test
  io.of('/listboard').emit('update board', info);
  socket.setEncoding('utf8');
  console.log("Co nguoi ket noi " );
  socket.on('data',function(data){
    console.log(socket.name,data);
    for(var i = 0;i<info.length;i++){
    //set nut di 2 4 6 8 va nut ban 5
      if(socket.name == info[i].name){
               if(data == '8')
                 info[i].locatey--;
               if(data == '4')
                 info[i].locatex--;
               if(data == '2')
                 info[i].locatey++;
               if(data == '6')
                 info[i].locatex++;
               if(info[i].locatex ==8)
                 info[i].locatex = 0;
               if(info[i].locatex ==-1)
                 info[i].locatex = 7;
               if(info[i].locatey ==8)
                 info[i].locatey = 0;
               if(info[i].locatey ==-1)
                 info[i].locatey = 7;
               if(data == '5')
                 info[i].isFire =true;
      }
    }
     io.of('/mapping2').emit('update map',info);
  });
  //Timeout neu mat ket noi voi board check moi 3s
  socket.setTimeout(3000);
socket.on('timeout', () => {
  
  socket.end();
});
// Timeout afk 5' bi vang
 socket.setTimeout(6000000);
  socket.on('timeout', () => {
    console.log('timeout');
    
    for(var i = 0;i < clients.length;i++){
      
      if(clients[i]==socket)
        clients.splice(i,1);
      
      if(info[i].name == socket.name)
        info.splice(i,1);
     
    }
    
    //update lai board
    io.of('/listboard').emit('update board', info);
    socket.end();
  });
  socket.on('error',function(err){});
  socket.on('close',function(err){
  console.log('mat ket noi board');
  
  //set var vi tri bang 0 khi disconnect va bang 1 khi connect de hien thong tin len terminal
    var vitri;
    for(var i = 0;i < clients.length;i++){
      
      if(clients[i]==socket)
        clients.splice(i,1);
      
      if(info[i].name == socket.name)
      {
        info.splice(i,1);
        vitri = i;
      }
    }
    io.of('/mapping2').emit('disconnected controller',vitri);
    console.log("mat ket noi");
    console.log(vitri);
    io.of('/listboard').emit('update board', info);  
  });
  
  
});
}
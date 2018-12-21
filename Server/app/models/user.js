var mongoose = require('mongoose');
var bcrypt   = require('bcrypt-nodejs');

//tao du lieu trong mongodb
var userSchema = mongoose.Schema({

    local            : {
        email        : String,
        password     : String,
    },
    locateboat       : [{
    x                :Number,
    y                :Number,
    isBoat           :Boolean,
    isBreak          :Boolean,
    isShoot          :Boolean
}],
    isTurn           :Boolean,
    enemyId          : String

});

//tao ma hoa password
userSchema.methods.generateHash = function(password) {
    return bcrypt.hashSync(password, bcrypt.genSaltSync(8), null);
};

//kiem tra mat khau trung khop
userSchema.methods.validPassword = function(password) {
    return bcrypt.compareSync(password, this.local.password);
};

//tao module ra cho app
module.exports = mongoose.model('User', userSchema);
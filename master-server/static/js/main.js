// var cur_temp = 15;//{{ cur_temp }};
const update_delay = 1000
function update_temp() {
  $.getJSON($SCRIPT_ROOT + '/_update_cur_temp', function(data) {
    $("#cur_temp").text(data.result);
    console.log(data.result);
  });
  return false;
}
window.setInterval(function(){
  update_temp()
}, update_delay);

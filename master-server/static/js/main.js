// var cur_temp = 15;//{{ cur_temp }};
let update_delay = 2000;
const RTU_id = 2;
const RTU2_id = 3;

var RTU_obj;
function update_temp() {
  $.getJSON($SCRIPT_ROOT + '/_update_cur_temp/' + RTU2_id.toString(), function(data) {
    // $("#cur_temp").text(data.cur_temp);
    // $("#cur_hum").text(data.cur_hum);
    RTU_obj = data;
    RTU_obj[4].sort(function(item2, item1){
      if (item1[0] != item2[0]) return item1[0] - item2[0]
      else if (item1[1] != item2[1]) return item1[1] - item2[1];
      else if (item1[2] != item2[2]) return item1[2] - item2[2];
      else if (item1[3] != item2[3]) return item1[3] - item2[3];
      else if (item1[4] != item2[4]) return item1[4] - item2[4];
      else if (item1[5] != item2[5]) return item1[5] - item2[5];
      else return 0;
    });
    str = "";
    for (let i = 0; i < Object.keys(RTU_obj[4]).length; i++) {
      str = str + "<tr><td>" +
      RTU_obj[4][i][0].toString() + "/" +
      RTU_obj[4][i][1].toString() + "/" +
      RTU_obj[4][i][2].toString() + "</td><td>" +
      RTU_obj[4][i][3].toString() + ":" +
      RTU_obj[4][i][4].toString() + ":" +
      RTU_obj[4][i][5].toString() + "</td><td>" +
      RTU_obj[4][i][6].toString() + "C " +
      RTU_obj[4][i][7].toString() + "%" +"</td></tr>";
    }
    $("#history").html(str);
  });
  return false;
}
update_temp();
window.setInterval(function(){
  update_temp()
}, update_delay);

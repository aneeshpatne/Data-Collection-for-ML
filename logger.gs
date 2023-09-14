function doGet(e) { 
  Logger.log( JSON.stringify(e) );
  var result = 'Ok';
  if (e.parameter == 'undefined') {
    result = 'No Parameters';
  }
  else {
    var sheet_id = '1WwMGdwaGbLfocR2NQSKotskTGjIykx_oCc3guMqP3QQ';  // Spreadsheet ID
    var sheet = SpreadsheetApp.openById(sheet_id).getActiveSheet();
    var newRow = sheet.getLastRow() + 1;            
    var rowData = [];
    var lastVal = sheet.getRange("A1:A").getValues();
    var id = lastVal.filter(String).length;
    rowData[0] = id;
    var Curr_Date = new Date();
    var date_ = Utilities.formatDate(Curr_Date,"IST","dd/MM/YYYY");
    rowData[1] = date_; // Date in column A
    var Curr_Time = Utilities.formatDate(Curr_Date, "IST", 'HH:mm:ss');
    rowData[2] = Curr_Time; // Time in column B
    for (var param in e.parameter) {
      Logger.log('In for loop, param=' + param);
      var value = stripQuotes(e.parameter[param]);
      Logger.log(param + ':' + e.parameter[param]);
      switch (param) {
        case 'temperature':
          rowData[3] = value; // Temperature in column C
          result = 'Temperature Written on column C'; 
          break;
        case 'humidity':
          rowData[4] = value; // Humidity in column D
          result += ' ,Humidity Written on column D'; 
          break;
        case 'pressure':
        rowData[5] = value; // Humidity in column D
          result += ' ,Pressure Written on column E'; 
          break;
        case 'light':
        rowData[6] = value; // Humidity in column D
          result += ' ,Light Written on column F'; 
          break;

        default:
          result = "unsupported parameter";
      }
    }
    Logger.log(JSON.stringify(rowData));
    var newRange = sheet.getRange(newRow, 1, 1, rowData.length);
    newRange.setValues([rowData]);
  }
  return ContentService.createTextOutput(result);
}
function stripQuotes( value ) {
  return value.replace(/^["']|['"]$/g, "");
}


if (!top.mainframe)
{
	document.write(
'<table width="100%" border="0" cellspacing="0" cellpadding="3">' +
'  <tr>' + 
'    <td class="b2">' +
'	    <a href="' + strHomeURL + '" target="_top">' + strBBSName +
'</a>(�����չ����������)' + 
'	    - ' + strDesc +
'	    - �Ķ�����' +
'    </td>' +
'  </tr>'
);
}
else
{
	document.write(
'<table width="100%" border="0" cellspacing="0" cellpadding="3">' +
'  <tr>' + 
'    <td class="b2"><a href="/mainpage.php">' + strBBSName + '</a>' +
'	    - ' + strDesc +
'	    - �Ķ�����' +
'    </td>' +
'  </tr>'
);
}

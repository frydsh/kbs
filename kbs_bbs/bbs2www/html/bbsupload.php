<?php
	/**
	 * This file lists boards to user.
	 * $Id$
	 */
	$filenames=array();
	$filesizes=array();
	global $errno;
	@$act_attachname=$_GET["attachname"];
	@$action=$_GET["act"];
    	$totalsize=0;
	require("funcs.php");
	if ($loginok !=1 )
		html_nologin();
	else
	{
		html_init("gb2312","ճ������");
		if (!valid_filename($act_attachname))
                    html_error_quit("������ļ���");
		$attachdir=ATTACHTMPPATH . "/" . $utmpkey;
		if ($action=="delete") {
			unlink($attachdir . "/" . "$act_attachname");
		} else if ($action=="add") {
			@$errno=$_FILES['attachfile']['error'];
			if ($errno==UPLOAD_ERR_OK) {
		        if ($_FILES['attachfile']['size']<=ATTACHMAXSIZE) {
				@mkdir($attachdir);
				if (is_uploaded_file($_FILES['attachfile']['tmp_name'])) {
			    			move_uploaded_file($_FILES['attachfile']['tmp_name'], 
			        			$attachdir . "/" . $act_attachname);
					}
				} else
					$errno=UPLOAD_ERR_FORM_SIZE;
			}
		}
		$filecount=0;
		if ($handle = @opendir($attachdir)) {
                    while (false != ($file = readdir($handle))) { 
                        if ($file[0]=='.')
                            continue;
                    	$filenames[] = $file;
                    	$filesizes[] = filesize($attachdir . "/" . $file);
                    	$totalsize+=$filesizes[$filecount];
                    	$filecount++;
			$allnames+=$file . ";";
                    }
                    closedir($handle);
                }
?>
<script language="JavaScript">
<!--

function SymError()
{
  return true;
}

window.onerror = SymError;

//-->
</script>

<style type="text/css">.txt01 {  font-family: "����"; font-size: 12px}
.txt02 {  font-family: "����"; font-size: 12px; color: #770260}
.txt-b01 { font-family: "����"; font-size: 12px; color: #770260 ; background-color: #C7A9EF; border-color: #C7A9EF #5C034A #551A49 #C9B3E7; border-style: solid; border-top-width: 1px; border-right-width: 1px; border-bottom-width: 1px; border-left-width: 1px}
.txt-b02 { font-family: "����"; font-size: 12px; color: #000000; background-color: #F5EAF9}
.txt-b03 { font-family: "����"; font-size: 12px; color: #770260; background-color: #F5EAF9}
.form01 {  font-family: "����"; font-size: 12px; height: 20px; letter-spacing: 3px}
.form02 {  font-size: 12px}
a:hover {  color: #FF0000; text-decoration: none}
.title01 { font-family: "����"; font-size: 16px; color: #770260 ; letter-spacing: 5px}
</style>

<script language=javascript>
function addsubmit() {
  var e1,e3;
  var e2;
  var pos=0;
  if (document.forms[0].elements["attachfile"].value == ""){
	alert('����ûѡ���ϴ��ĸ���');
 	return false;
  } else {
        e1=e3=document.forms[0].elements["attachfile"].value;
        pos=e1.indexOf('&',0);
        if (pos!=-1)
            e3=e1.substring(0,pos)+e1.substring(pos+1,document.forms[0].elements["attachfile"].value.length);
        e2="bbsupload.php?act=add&attach=bbsfoot0.htm&attachname=bbsfoot0.htm&attachfile="+e3+"&sid=NWs2YzFuNDA5MDEwNTA0MDAw";
        document.forms[0].action=e2;  
        document.forms[0].submit();
  }
}

function deletesubmit() {
  var e2;
  e2="bbsupload.php?act=delete&attach=bbsfoot0.htm&attachname=bbsfoot0.htm&removefile="+document.forms[1].elements["removefile"].value+"&sid=NWs2YzFuNDA5MDEwNTA0MDAw";
  document.forms[1].action=e2;
  document.forms[1].submit();
}

function clickclose() {
	if (document.forms[0].elements["attachfile"].value == "") return window.close();
	else if (confirm("����д���ļ�������û�����ء��Ƿ�ȷ�Ϲرգ�")==true) return window.close();
	return false;
}
<!--
        opener.document.forms["attach"].elements["attach"].value = <?php echo "\"$allnemes\""; ?>;
//-->
</script>
<body bgcolor="#FFFFFF"  background="/images/rback.gif">
<?php
                if ($action=="add") {
                	if ($_FILES['attachfile']['size']+$totalsize>ATTACHMAXSIZE) {
                		unlink($attachdir . "/" . $act_attachname);
                		$errno=UPLOAD_ERR_FORM_SIZE;
                	}
                	switch ($errno) {
                	case UPLOAD_ERR_OK:
                		echo "�ļ����سɹ���";
                		break;
                	case UPLOAD_ERR_INI_SIZE:
                	case UPLOAD_ERR_FORM_SIZE:
                		echo "�ļ�����Ԥ���Ĵ�С" . sizestr(ATTACHMAXSIZE) . "�ֽ�";
                		break;
                	case UPLOAD_ERR_PARTIAL:
                		echo "�ļ��������";
                		break;
                	case UPLOAD_ERR_NO_FILE:
                		echo "û���ļ��ϴ���";
                		break;
                	default:
                		echo "δ֪����";
                	}
                	echo "<br />";
                }
?>
<form name="addattach" method="post" ENCTYPE="multipart/form-data" align="left" action="">
  <table border="0" cellspacing="2" class="txt-b03">
    <tr> 
      <td height="2"> 
        <table border="0">
          <tr> 
            <td colspan="2" height="13" class="txt02"> <font>1���㡰<font color="#FF0000">���</font>����ť���ҵ�����Ҫճ���ĸ����ļ���</font> 
            </td>
          </tr>
          <tr> 
            <td colspan="2" height="25" class="form01"> 
              <input type="hidden" name="MAX_FILE_SIZE" value=<?php echo(ATTACHMAXSIZE);?>>
              <input type="file" name="attachfile" size="20" value class="form02">
            </td>
          </tr>
        </table>
      </td>
    </tr>
    <tr> 
      <td> 
        <table border="0" width="500">
          <tr> 
            <td width="420" class="txt02">2��ѡ��һ���ļ��󣬵㡰<font color="#FF0000">ճ��</font>����ť�������ļ�����������:</td>
            <td width="80"> 
              <input type="button" width="61"
          height="21" value="ճ��" border="0" onclick="addsubmit()" class="form01" >
            </td>
          </tr>
        </table>
      </td>
    </tr>
    <tr> 
      <td class="txt02"> 3�����ж�������ļ�Ҫճ�����ظ�1��2���裻</td>
    </tr>
    <tr> 
      <td> 
        <table border="0" width="500">
          <tr> 
            <td width="420" class="txt02">4�����еĸ���ճ����Ϻ󣬵㡰<font color="#FF0000">���</font>����ť���ء�</td>
            <td width="80"> 
              <input type="button"  width="61"
          height="21" value="���" border="0" onclick="return clickclose()" class="form01">
            </td>
          </tr>
        </table>
      </td>
    </tr>
  </table>
</form><form name="deleteattach" ENCTYPE="multipart/form-data" method="post" align="left" action=""> 
<table border="0" width="500" cellspacing="2">
  <tr> 
    <td width="423"><font color="#804040"> 
      <select name="removefile" cols="40" size="1" class="form02">
<?php
      foreach ($filenames as $file)
        printf("<option value=\"%s\">%s</option>",$file,$file);
?>
      </select>
      </font></td>
    <td width="77"> 
      <input type="button" width="61" height=21 value="ɾ��" onclick="return deletesubmit()" class="form01">
    </td>
  </tr>
</table>
  <table border="0" cellspacing="2" class="txt-b03">
    <tr><td>
        <table border="0" width="500">
          <tr> 
            <td width="150" class="txt01" align="right">���ڸ����ļ�����Ϊ��</td>
            <td width="350" class="txt01"><font color="#FF0000"><b>&nbsp;&nbsp;<?php echo sizestring($totalsize); ?>�ֽ�</b></font></td>
  	</tr>
          <tr> 
            <td width="150" class="txt01" align="right">ע�⣺��������������ܳ�����</td>
            <td width="350" class="txt01"><font color="#FF0000"><b>&nbsp;&nbsp;<?php echo sizestring(ATTACHMAXSIZE); ?>�ֽ�</b></font></td>
  	</tr>
  	</table>
  </td></tr>
</table>
<table width="75%" border="0" align="center" cellpadding="0" cellspacing="0">
</table>
<div align="center"> 
  <center>
    <p>��</p>
  </center>
</div></form>
</body>
</html>
<?php
	} // nologin else
?>

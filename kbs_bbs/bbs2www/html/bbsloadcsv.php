<?php
	/**
	 * 
	 * $Id$
	 */
	global $errno;
	@$action=$_GET["act"];
	require("funcs.php");
	if ($loginok !=1 )
		html_nologin();
	else
	{
		html_init("gb2312","csv����");
		$attachdir=bbs_getattachtmppath($currentuser["userid"] ,$utmpnum);
		@mkdir($attachdir);
		if ($action=="add") {
			@$errno=$_FILES['attachfile']['error'];
			if ($errno==UPLOAD_ERR_OK) {
				$buf=$_FILES['attachfile']['name'];
				
				$tok = strtok($buf,"/\\");
				$act_attachname="";
				while ($tok) {
					$act_attachname=$tok;
    					$tok = strtok("/\\");
				}
				$act_attachname=strtr($act_attachname,$filename_trans);
				$act_attachname=substr($act_attachname,-60);
				if ($act_attachname!="") {
			        	if ($_FILES['attachfile']['size']>ATTACHMAXSIZE) 
							$errno=UPLOAD_ERR_FORM_SIZE;
				} else
					$errno=100;
			}
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
.form01 {  font-family: "����"; font-size: 12px; height: 20px; letter-spacing: 3px}
.form02 {  font-size: 12px}
</style>

<body bgcolor="#FFFFFF"  background="/images/rback.gif">
<?php
                if ($action=="add") {
                	switch ($errno) {
                	case UPLOAD_ERR_OK:
						$tmpfilename=tempnam($attachdir,"att");
						if (is_uploaded_file($_FILES['attachfile']['tmp_name'])) {
			    			if( move_uploaded_file($_FILES['attachfile']['tmp_name'], 
			        			$tmpfilename) == FALSE ){
								echo "����ʧ��";
							}else{
               					echo "�ļ�����ɹ���";
								$ret = bbs_csv_to_al($tmpfilename);
								echo "������".$ret."��";
								@unlink($tmpfilename);
               					break;
							}
						}
						echo "�����ļ�ʧ�ܣ�";
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
                	case 100:
                		echo "��Ч���ļ�����";
                	default:
                		echo "δ֪����";
                	}
                	echo "<br />";
					html_normal_quit();
                }
?>
<script language=javascript>
function addsubmit() {
  var e1,e3;
  var e2;
  var pos=0;
  var obj=document.forms[0].elements["attachfile"];
  if (!obj) return true;
  if (obj.value == ""){
	alert('����ûѡ���ϴ����ļ�');
 	return false;
  } else {
        e2="bbsloadcsv.php?act=add";
        document.forms[0].action=e2;  
        document.forms[0].submit();
  }
}

function clickclose() {
	return window.close();
}
</script>
��ѡ��Ҫ�����csv�ļ�:<br>
<form name="addattach" method="post" ENCTYPE="multipart/form-data" align="left" action="">
              <input type="hidden" name="MAX_FILE_SIZE" value=<?php echo(ATTACHMAXSIZE);?>>
              <input type="file" name="attachfile" size="20" value class="form02">
              <input type="button" width="61"
          height="21" value="����" border="0" onclick="addsubmit()" class="form01" >
</form>
</body>
</html>
<?php
	} // nologin else
?>

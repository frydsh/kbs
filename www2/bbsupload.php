<?php
	require("www2-funcs.php");
	require("www2-bmp.php");
	$sessionid = login_init(TRUE);
	assert_login();
	
	@$action=$_GET["act"];
	$msg = "";
	if ($action=="delete") {
		@$act_attachname=$_GET["attachname"];
		settype($act_attachname, "string");
		$ret = bbs_upload_del_file($act_attachname);
		if ($ret) $msg = bbs_error_get_desc($ret);
		else $msg = "ɾ���ļ��ɹ�";
	} else if ($action=="add") {
		if (isset($_FILES['attachfile'])) {
			@$errno=$_FILES['attachfile']['error'];
		} else {
			$errno = UPLOAD_ERR_PARTIAL;
		}
		switch ($errno) {
		case UPLOAD_ERR_OK:
			$ofile = $_FILES['attachfile']['tmp_name'];
			$oname = $_FILES['attachfile']['name'];
			if (!is_uploaded_file($ofile)) {
				die;
			}
			if (compress_bmp($ofile, $oname)) {
				$msg = "���� BMP ͼƬ���Զ�ת���� PNG ��ʽ��";
			}
			$ret = bbs_upload_add_file($ofile, $oname);
			if ($ret) $msg = bbs_error_get_desc($ret);
			else $msg .= "�ļ����سɹ���";
			break;
		case UPLOAD_ERR_INI_SIZE:
		case UPLOAD_ERR_FORM_SIZE:
			$msg = "�ļ��������� " . sizestring(BBS_MAXATTACHMENTSIZE) . " �ֽ�";
			break;
		case UPLOAD_ERR_PARTIAL:
			$msg = "�ļ��������";
			break;
		case UPLOAD_ERR_NO_FILE:
			$msg = "û���ļ��ϴ���";
			break;
		default:
			$msg = "δ֪����";
		}
	}
	$attachments = bbs_upload_read_fileinfo();
	$filecount = count($attachments);
	$allnames = array();$totalsize=0;
	for($i=0;$i<$filecount;$i++) {
		$allnames[] = $attachments[$i]["name"];
		$totalsize += $attachments[$i]["size"];
	}
	$allnames=implode(",",$allnames);
	page_header("�ϴ�����", FALSE);
?>
<body>
<script type="text/javascript">
<!--
function addsubmit() {
	var obj=document.forms[0].elements["attachfile"];
	if (!obj) return true;
	if (obj.value == ""){
		alert('����ûѡ���ϴ��ĸ���');
		return false;
	} else {
		var e2="bbsupload.php?act=add";
		getObj("winclose").style.display = "none";
		document.forms[0].action=e2;
		document.forms[0].paste.value='���������У����Ժ�...';
		document.forms[0].paste.disabled=true;
		document.forms[0].submit();
		return true;
	}
}

function deletesubmit(f) {
	var e2="bbsupload.php?act=delete&attachname="+f;
	document.forms[1].action=e2;
	document.forms[1].submit();
}

function clickclose() {
	if (document.forms[0].elements["attachfile"].value == "") return window.close();
	else if (confirm("����д���ļ�������û�����ء��Ƿ�ȷ�Ϲرգ�")==true) return window.close();
	return false;
}

if (opener) {
	opener.document.forms["postform"].elements["attachname"].value = "<?php echo $allnames; ?>";
} else {
	addBootFn(function() { getObj("winclose").style.display = "none"; });
}
//-->
</script>
<div style="width: 550px; margin: 1em auto;">
<?php if ($msg) echo "<font color='red'> ��ʾ��".$msg."</font>"; ?>
<form name="addattach" method="post" ENCTYPE="multipart/form-data" class="left" action="">
<?php if ($sessionid) echo "<input type='hidden' name='sid' value='$sessionid' />"; ?>
ѡ����Ҫ�ϴ����ļ�����ϴ��������ж�������ļ�Ҫ�ϴ������ظ�������裩<br/>
<?php
	if ($filecount<BBS_MAXATTACHMENTCOUNT) {
?>
		<input type="hidden" name="MAX_FILE_SIZE" value="<?php echo(BBS_MAXATTACHMENTSIZE);?>" />
		<input type="file" name="attachfile" size="20" />
		<input type="button" value="�ϴ�" name="paste" onclick="addsubmit();" />
<?php
	} else {
?>
		��������������
		<input type="hidden" name="attachfile" />
<?php
	}
?>
&nbsp;&nbsp;&nbsp;<input type="button" id="winclose" value="�ϴ����, �رմ���" onclick="return clickclose()" />
<p>�����ļ�������<?php echo sizestring($totalsize); ?> �ֽ�,
���ޣ�<?php echo sizestring(BBS_MAXATTACHMENTSIZE); ?> �ֽ�,
�����ϴ���<font color="#FF0000"><b><?php $rem = BBS_MAXATTACHMENTSIZE-$totalsize; 
	if ($rem < 0) $rem = 0; echo sizestring($rem); ?> �ֽ�</b></font>.</p>
</form>

<form name="deleteattach" ENCTYPE="multipart/form-data" method="post" class="left" action=""> 
<?php if ($sessionid) echo "<input type='hidden' name='sid' value='$sessionid' />"; ?>
<ol style="padding-left: 2em; margin-left: 0em;">�Ѿ��ϴ��ĸ����б�: (������ϴ� <?php echo BBS_MAXATTACHMENTCOUNT; ?>
 ��, �����ϴ� <font color="#FF0000"><b><?php echo (BBS_MAXATTACHMENTCOUNT-$filecount); ?></b></font> ��)
<?php
	for($i=0;$i<$filecount;$i++) {
		$f = $attachments[$i];
		echo "<li>".$f["name"]." (".sizestring($f["size"])."�ֽ�) <a href=\"javascript:deletesubmit('".$f["name"]."');\">ɾ��</a></li>";
	}
?>
</ol>
</form>
</div>
</body>
</html>

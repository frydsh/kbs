<?php
	/**
	 * This file lists articles to user.
	 * $Id$
	 */
	require("funcs.php");
	require("boards.php");
	if ($loginok != 1)
		html_nologin();
	else
	{
		html_init("gb2312");
		if (isset($_GET["board"]))
			$board = $_GET["board"];
		else
			html_error_quit("�����������");
		// ����û��ܷ��Ķ��ð�
		$brdarr = array();
		$brdnum = bbs_getboard($board, $brdarr);
		if ($brdnum == 0)
			html_error_quit("�����������");
		bbs_set_onboard($brcnum,1);
		$usernum = $currentuser["index"];
		if (bbs_checkreadperm($usernum, $brdnum) == 0)
			html_error_quit("�����������");
		if(bbs_checkpostperm($usernum, $brdnum) == 0) {
                    if (!strcmp($currentuser["userid"],"guest"))
		      html_error_quit("����ע���ʺ�");
                    else 
		      html_error_quit("�������������������Ȩ�ڴ���������������");
                }
		if (bbs_is_readonly_board($brdarr))
			html_error_quit("������ֻ����������������");
		if (isset($_GET["reid"]))
		{
			$reid = $_GET["reid"];
			if(bbs_is_noreply_board($brdarr))
				html_error_quit("����ֻ�ɷ�������,���ɻظ�����!");
		}
		else {
			$reid = 0;
		}
		settype($reid, "integer");
		$articles = array();
		if ($reid > 0)
		{
			$num = bbs_get_records_from_id($brdarr["NAME"], $reid,$dir_modes["NORMAL"],$articles);
			if ($num == 0)
			{
				html_error_quit("����� Re �ı��");
			}
			if ($articles[1]["FLAGS"][2] == 'y')
				html_error_quit("���Ĳ��ɻظ�!");
		}
		$brd_encode = urlencode($brdarr["NAME"]);
	}
?>
<body>
<center>
<?php echo BBS_FULL_NAME; ?> -- �������� [ʹ����: <?php echo $currentuser["userid"]; ?>]
<hr class="default" />
<form name="postform" method="post" action="bbssnd.php?board=<?php echo $brd_encode; ?>&reid=<?php echo $reid; ?>">
<table border="1">
<tr>
<td>
<?php
		$notes_file = bbs_get_vote_filename($brdarr["NAME"], "notes");
		$fp = FALSE;
		if(file_exists($notes_file))
		{
		    $fp = fopen($notes_file, "r");
		    if ($fp == FALSE)
		    {
    	    	$notes_file = "vote/notes";
                if(file_exists($notes_file))
	    		    $fp = fopen($notes_file, "r");
    		}
		}
		if ($fp == FALSE)
    	{
?>
<font color="green">����ע������: <br />
����ʱӦ���ؿ������������Ƿ��ʺϹ������Ϸ������������ˮ��лл���ĺ�����<br/></font>
<?php
		}
        else
		{
		    fclose($fp);
			echo bbs_printansifile($notes_file);
		}
?>
</td>
</tr>
<tr><td>
����: <?php echo $currentuser["userid"]; ?><br />
<?php
		if ($reid)
		{
	        if(!strncmp($articles[1]["TITLE"],"Re: ",4))$nowtitle = $articles[1]["TITLE"];
	        else
	            $nowtitle = "Re: " . $articles[1]["TITLE"];
?>
����: <input type="text" name="title" size="40" maxlength="100" value="<?php echo $nowtitle; ?>

" /><br />
<?php
		}
		else
		{
?>
����: <input type="text" name="title" size="40" maxlength="100" value=""><br />
<?php
		}
?>
����: [<?php echo $brd_encode; ?>]<br />
<?php
		if (bbs_is_attach_board($brdarr))
		{
?>
����: <input type="text" name="attachname" size="50" value="" disabled="disabled" />
<?php
		}
?>
ʹ��ǩ���� <select name="signature">
<?php
		if ($currentuser["signature"] == 0)
		{
?>
<option value="0" selected="selected">��ʹ��ǩ����</option>
<?php
		}
		else
		{
?>
<option value="0">��ʹ��ǩ����</option>
<?php
			for ($i = 1; $i < 6; $i++)
			{
				if ($currentuser["signature"] == $i)
				{
?>
<option value="<?php echo $i; ?>" selected="selected">�� <?php echo $i; ?> ��</option>
<?php
				}
				else
				{
?>
<option value="<?php echo $i; ?>">�� <?php echo $i; ?> ��</option>
<?php
				}
			}
		}
?>
</select>
 [<a target="_balnk" href="bbssig.php">�鿴ǩ����</a>]
<input type="checkbox" name="outgo" value="1" CHECKED/>ת��<br />
<textarea name="text"  onkeydown='if(event.keyCode==87 && event.ctrlKey) {document.postform.submit(); return false;}'  onkeypress='if(event.keyCode==10) return document.postform.submit()' rows="20" cols="80" wrap="physical">
<?php
    if($reid > 0){
    $filename = $articles[1]["FILENAME"];
    $filename = "boards/" . $board . "/" . $filename;
	if(file_exists($filename))
	{
	    $fp = fopen($filename, "r");
        if ($fp) {
		    $lines = 0;
            $buf = fgets($fp,256);       /* ȡ����һ���� ���������µ� ������Ϣ */
			$end = strrpos($buf,")");
			$start = strpos($buf,":");
			if($start != FALSE && $end != FALSE)
			    $quser=substr($buf,$start+2,$end-$start-1);

            echo "\n�� �� " . $quser . " �Ĵ������ᵽ: ��\n";
            for ($i = 0; $i < 3; $i++) {
                if (($buf = fgets($fp,500)) == FALSE)
                    break;
            }
            while (1) {
                if (($buf = fgets($fp,500)) == FALSE)
                    break;
                if (strncmp($buf, ": ��", 4) == 0)
                    continue;
                if (strncmp($buf, ": : ", 4) == 0)
                    continue;
                if (strncmp($buf, "--\n", 3) == 0)
                    break;
                if (strncmp($buf,'\n',1) == 0)
                    continue;
                if (++$lines > 10) {
                    echo ": ...................\n";
                    break;
                }
                /* */
                if (stristr($buf, "</textarea>") == FALSE)  //filter </textarea> tag in the text
                    echo ": ". $buf;
            }
            fclose($fp);
        }
    }
}
?>
</textarea></td></tr>
<tr><td class="post" align="center">
<input type="submit" value="����" />
<input type="reset" value="���" />
<?php
		if (bbs_is_attach_board($brdarr))
		{
?>
<input type="button" name="attach22" value="����" onclick="return GoAttachWindow()" />
<?php
		}
?>
<script language="JavaScript">
<!--
   function GoAttachWindow(){

   	var hWnd = window.open("bbsupload.php","_blank","width=600,height=300,scrollbars=yes");

	if ((document.window != null) && (!hWnd.opener))

		   hWnd.opener = document.window;

	hWnd.focus();

   	return false;

   }
-->
</script>
</td></tr>
</table></form>

<center><a href="/bbsdoc.php?board=<?php echo $brd_encode; ?>">��������</a></center>

</html>
<?php
?>

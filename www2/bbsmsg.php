<?php
    require("www2-funcs.php");
    login_init();
	page_header("�쿴����ѶϢ");
	assert_login();

	$msgs = bbs_getwebmsgs();

	if( $msgs <= 0 ) html_error_quit("ϵͳ����");

	$i=0;
?>
<script type="text/javascript"><!--
var ta = new tabWriter(1,'main wide fixed',0,[['���','6%','center'],['ʱ��','20%',0],['����','6%','center'],['����','10%',0],['����',0,0]]);
<?php
	foreach( $msgs as $msg ){
?>
ta.r('<?php echo date("Y/m/j H:i:s", $msg["TIME"]);?>','<?php
if($msg["SENT"]) echo '<a href="bbssendmsg.php?destid='.$msg["ID"].'">��</a>'; else echo '��';?>','<?php echo $msg["ID"];?>','<?php
echo htmlformat($msg["content"],true);?> ');
<?php
	}
?>
ta.t();
//-->
</script>
<div class="oper">
<a onclick="return confirm('�����Ҫ�������ѶϢ��?')" href="bbsdelmsg.php">�������ѶϢ</a> <a href="bbsmailmsg.php">�Ļ�������Ϣ</a>
</div>
<?php
	page_footer();
?>

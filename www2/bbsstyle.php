<?php
	require("www2-funcs.php");
	login_init();
	toolbox_header("�����޸�");

	if (isset($_GET['do'])) {
		$new_wwwparams = @$_COOKIE["WWWPARAMS"];
		if (strcmp($currentuser["userid"], "guest") && $new_wwwparams) {
			bbs_setwwwparameters($new_wwwparams); /* TODO: return value ? */
		}
	}
?>
<script type="text/javascript"><!--
	var settings = {"sizer": 3, "pager": 4, "hot": 5}; /* faint IE5 */
	function setInd(n, v) {
		var ff = getObj(n + 'F');
		var tt = getObj(n + 'T');
		/* some users might not have bold font... that's why I add underline */
		if (ff) {
			ff.style.fontWeight = v ? 'normal' : 'bold';
			ff.style.textDecoration = v ? 'none' : 'underline';
		}
		if (tt) {
			tt.style.fontWeight = v ? 'bold' : 'normal';
			tt.style.textDecoration = v ? 'underline' : 'none';
		}
	}
	function adjSet(n, v) {
		var idx = settings[n];
		if (n == "sizer") {
			getObj("fontSizer").style.display = v ? "block" : "none";
		}
		setInd(n, v);
		var mask = 1 << idx;
		saveParaCookie(v ? mask : 0, mask);
	}
	function KCNymsw() {
		alert('��û�����������... ��Ҫ�𣿲���Ҫ����ȥ sysop �溰����');
	}
	function setStyleDiv(cssID, setImg) {
		for (var i=0; i<32; i++) {
			var o = getObj("stylediv" + i);
			if (o) {
				o.style.borderColor = (i!=cssID) ? 'white' : 'blue';
				getObj("stylethumb" + i).src = "images/" + i + "/thumb.jpg";
			} else break;
		}
	}
	function chkStyle(cssID) {
		saveParaCookie(cssID << 7, 0xF80);
		setStyleDiv(getCssID(), 0);
	}
	function applyStyle() {
		resetCss();
		var ff = top.window["menu"]; if (ff) ff.resetCss();
		ff = top.window["toogle"]; if (ff) ff.resetCss();
		ff = top.window["f4"]; if (ff) ff.resetCss();
	}
	addBootFn(function() {
		var c = readParaCookie();
		for (var n in settings) {
			var i = settings[n];
			var v = c & (1 << i);
			setInd(n, v);
		}
		setStyleDiv(getCssID(), 1);
		if (!isLogin()) {
			getObj("cmdSave").disabled = true;
			getObj("idpsave").style.visibility = "hidden";
		}
	});
//-->
</script>
<style type="text/css">
form.stylesel {
	text-align: center;
	margin: 0px auto 10px auto;
	width: 500px;
}
div.stylesel {
	float:left;
	margin: 10px;
	padding: 7px;
	border: 3px solid white;
}
div.stylesel img {
	width: 200px;
	height: 150px;
	margin-bottom: 10px;
	border: 0;
}
</style>
<form action="?" class="medium align" method="get">
<input type="hidden" name="do" value="1" />
	<fieldset><legend>����ѡ��</legend>
		<div class="inputs">
			<label>�����С:</label>
				<span class="clickable" onclick="sizer(1)">�Ŵ�</span>
				<span class="clickable" onclick="sizer(-1)">��С</span>
				<span class="clickable" onclick="sizer(0)">�ָ�</span>
			<br/>
			<label>���������:</label>
				<span class="clickable" onclick="adjSet('sizer', 0)" id="sizerF">����</span>
				<span class="clickable" onclick="adjSet('sizer', 1)" id="sizerT">��ʾ</span>
			<br/>
			<label>���ϽǷ�ҳ����:</label>
				<span class="clickable" onclick="adjSet('pager', 0)" id="pagerF">����</span>
				<span class="clickable" onclick="adjSet('pager', 1)" id="pagerT">��ʾ</span>
			<br/>
<?php if (defined('BBS_NEWPOSTSTAT')) { ?>
			<label>���Ż������:</label>
				<span class="clickable" onclick="adjSet('hot', 0)" id="hotF">�·�</span>
				<span class="clickable" onclick="adjSet('hot', 1)" id="hotT">�Ϸ�</span>
				<span class="clickable" onclick="KCNymsw();">�ر�</span>
			<br/>
<?php } ?>
		</div>
	</fieldset>
</form>
<form action="?" class="stylesel" method="get">
<input type="hidden" name="do" value="1" />
	<fieldset><legend>���淽��:</legend>
		<div align="center">
<?php
	// ������ʾÿ�����淽��������ͼ
	$stylecount = count($style_names);
	$ret = "";
	for($i=0;$i<$stylecount;$i++)
	{
		$ret .= "<div id='stylediv{$i}' class='stylesel clickable' onClick='chkStyle($i);'>";
		$ret .= "<img id='stylethumb{$i}' title='������...' src='' /><br/>{$style_names[$i]}</div>";
	}
	print($ret);
?>
		</div>
		<div class="oper clear"><input type="button" onclick="applyStyle();" value="����Ӧ�ý��淽��"/><br/>
			[ ������������ť���޸Ķ���һ������ĵ�ҳ����Ч��]</div>
	</fieldset>
	<div class="oper"><input id="cmdSave" type="submit" value="��������"/> &nbsp; <input type="button" onclick="history.go(-1);" value="���ٷ���"/><br/>
		<span id="idpsave">[ �� �������� �Ա�֤ÿ�ε�¼��ʹ��������á�]</span></div>
</form>
<?php
	page_footer();
?>

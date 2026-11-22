#include "NexoKioskLanguage.h"

namespace
{
    struct FTranslation
    {
        const TCHAR* Key;
        const TCHAR* Malay;
        const TCHAR* Tamil;
        const TCHAR* Chinese;

        const TCHAR* Get(ENexoKioskLanguage Language) const
        {
            switch (Language)
            {
            case ENexoKioskLanguage::Malay: return Malay;
            case ENexoKioskLanguage::Tamil: return Tamil;
            case ENexoKioskLanguage::Chinese: return Chinese;
            default: return Key;
            }
        }
    };

    const FTranslation Labels[] = {
        {TEXT("TXT_BTN_HdrLang"), TEXT("Bahasa Melayu"), TEXT("தமிழ்"), TEXT("简体中文")},
        {TEXT("TXT_BTN_HdrHelp"), TEXT("Bantuan"), TEXT("உதவி"), TEXT("帮助")},
        {TEXT("TXT_HdrSub"), TEXT("Naik MRT dengan yakin"), TEXT("நம்பிக்கையுடன் பயணியுங்கள்"), TEXT("轻松搭乘地铁")},
        {TEXT("TXT_P0Title"), TEXT("Perjalanan anda bermula di sini"), TEXT("உங்கள் பயணம் இங்கே தொடங்குகிறது"), TEXT("从这里开始您的旅程")},
        {TEXT("TXT_P0Sub"), TEXT("Beli tiket, tambah nilai atau dapatkan bantuan."), TEXT("பயணச்சீட்டு வாங்குங்கள், மதிப்பைச் சேருங்கள் அல்லது உதவி பெறுங்கள்."), TEXT("购买车票、充值或寻求帮助。")},
        {TEXT("TXT_P0Kicker"), TEXT("MULA PERJALANAN"), TEXT("பயணத்தைத் தொடங்குங்கள்"), TEXT("开始旅程")},
        {TEXT("TXT_P0HeroH"), TEXT("Saya mahu pergi ke suatu tempat"), TEXT("நான் ஓர் இடத்திற்குச் செல்ல விரும்புகிறேன்"), TEXT("我要去一个地方")},
        {TEXT("TXT_P0HeroSub"), TEXT("Pilih destinasi, kemudian jenis tiket."), TEXT("செல்லும் இடத்தையும் சீட்டு வகையையும் தேர்ந்தெடுங்கள்."), TEXT("先选目的地，再选车票类型。")},
        {TEXT("TXT_P0Foot"), TEXT("Panduan audio • Teks besar • Bantuan kakitangan"), TEXT("ஒலி வழிகாட்டி • பெரிய எழுத்து • ஊழியர் உதவி"), TEXT("语音指引 • 大字 • 工作人员协助")},
        {TEXT("TXT_BTN_PlanTrip"), TEXT("Rancang perjalanan →"), TEXT("பயணத்தைத் திட்டமிடுங்கள் →"), TEXT("规划路线 →")},
        {TEXT("TXT_BTN_TopUp"), TEXT("Tambah nilai kad"), TEXT("அட்டைக்கு மதிப்பு சேர்க்கவும்"), TEXT("交通卡充值")},
        {TEXT("TXT_BTN_HomeHelp"), TEXT("Minta bantuan"), TEXT("உதவி கேளுங்கள்"), TEXT("寻求帮助")},
        {TEXT("TXT_P1Kick"), TEXT("DESTINASI / 1 DARIPADA 4"), TEXT("செல்லும் இடம் / 1 / 4"), TEXT("目的地 / 第 1 步，共 4 步")},
        {TEXT("TXT_P1Title"), TEXT("Ke mana anda hendak pergi?"), TEXT("நீங்கள் எங்கே செல்ல விரும்புகிறீர்கள்?"), TEXT("您要去哪里？")},
        {TEXT("TXT_P1Sub"), TEXT("Pilih stesen di bawah."), TEXT("கீழே உள்ள நிலையத்தைத் தேர்ந்தெடுங்கள்."), TEXT("请选择下方的车站。")},
        {TEXT("TXT_P1Search"), TEXT("Cari stesen atau kod stesen"), TEXT("நிலையம் அல்லது நிலையக் குறியீட்டைத் தேடுங்கள்"), TEXT("搜索车站或车站编号")},
        {TEXT("TXT_P1Sec"), TEXT("STESEN BERDEKATAN"), TEXT("அருகிலுள்ள நிலையங்கள்"), TEXT("附近车站")},
        {TEXT("TXT_BTN_DestBugis"), TEXT("Bugis (EW12)"), TEXT("பூகிஸ் (EW12)"), TEXT("武吉士 (EW12)")},
        {TEXT("TXT_BTN_DestCityHall"), TEXT("City Hall (NS25)"), TEXT("சிட்டி ஹால் (NS25)"), TEXT("政府大厦 (NS25)")},
        {TEXT("TXT_BTN_DestMarinaBay"), TEXT("Marina Bay (NS27)"), TEXT("மரினா பே (NS27)"), TEXT("滨海湾 (NS27)")},
        {TEXT("TXT_BTN_DestUnavailable"), TEXT("Jurong East (laluan tidak tersedia)"), TEXT("ஜூரோங் ஈஸ்ட் (வழித்தடம் கிடைக்கவில்லை)"), TEXT("裕廊东（路线暂不可用）")},
        {TEXT("TXT_P2RKick"), TEXT("SEMAK LALUAN"), TEXT("வழித்தடச் சரிபார்ப்பு"), TEXT("核对路线")},
        {TEXT("TXT_P2RTitle"), TEXT("Semak laluan anda"), TEXT("உங்கள் வழித்தடத்தைப் பாருங்கள்"), TEXT("查看您的路线")},
        {TEXT("TXT_P2RSub"), TEXT("Pastikan laluan ini betul sebelum membeli."), TEXT("வாங்குவதற்கு முன் வழித்தடத்தைச் சரிபாருங்கள்."), TEXT("购票前请确认路线。")},
        {TEXT("TXT_BTN_P2RChange"), TEXT("Tukar destinasi"), TEXT("செல்லும் இடத்தை மாற்றுங்கள்"), TEXT("更改目的地")},
        {TEXT("TXT_BTN_P2RNext"), TEXT("Pilih tiket →"), TEXT("பயணச்சீட்டைத் தேர்ந்தெடுங்கள் →"), TEXT("选择车票 →")},
        {TEXT("TXT_P3Kick"), TEXT("TIKET / 2 DARIPADA 4"), TEXT("பயணச்சீட்டு / 2 / 4"), TEXT("车票 / 第 2 步，共 4 步")},
        {TEXT("TXT_P3Title"), TEXT("Pilih jenis tiket"), TEXT("பயணச்சீட்டு வகையைத் தேர்ந்தெடுங்கள்"), TEXT("选择车票类型")},
        {TEXT("TXT_P3Sub"), TEXT("Bandingkan pilihan tiket di bawah."), TEXT("கீழே உள்ள சீட்டு வகைகளை ஒப்பிடுங்கள்."), TEXT("比较下方的车票选项。")},
        {TEXT("TXT_P3FareInfo"), TEXT("Harga untuk simulasi sahaja."), TEXT("இவை உருவகப் பயணக் கட்டணங்கள்."), TEXT("票价仅供模拟使用。")},
        {TEXT("TXT_BTN_PickOneWay"), TEXT("Sehala\nSatu perjalanan ke destinasi anda."), TEXT("ஒருவழிச் சீட்டு\nசெல்லும் இடத்திற்கு ஒரு பயணம்."), TEXT("单程票\n一次前往目的地。")},
        {TEXT("TXT_BTN_PickRoundTrip"), TEXT("Pergi balik\nKe destinasi dan kembali ke Orchard."), TEXT("இருவழிச் சீட்டு\nசென்றுவிட்டு ஆர்ச்சர்டுக்குத் திரும்பலாம்."), TEXT("往返票\n前往目的地并返回乌节。")},
        {TEXT("TXT_BTN_PickDayPass"), TEXT("Pas sehari (S$12)\nPerjalanan tanpa had hingga tengah malam."), TEXT("ஒருநாள் அனுமதிச்சீட்டு (S$12)\nநள்ளிரவு வரை வரம்பற்ற பயணம்."), TEXT("一日通票（S$12）\n午夜前不限次数乘车。")},
        {TEXT("TXT_P4Kick"), TEXT("SEMAK / 3 DARIPADA 4"), TEXT("சரிபார்ப்பு / 3 / 4"), TEXT("核对 / 第 3 步，共 4 步")},
        {TEXT("TXT_P4Title"), TEXT("Semua maklumat betul?"), TEXT("அனைத்தும் சரியாக உள்ளதா?"), TEXT("信息都正确吗？")},
        {TEXT("TXT_P4Sub"), TEXT("Semak tempahan sebelum membayar."), TEXT("பணம் செலுத்துமுன் விவரங்களைச் சரிபாருங்கள்."), TEXT("付款前请核对订单。")},
        {TEXT("TXT_BTN_ReviewEditDest"), TEXT("Ubah laluan"), TEXT("வழித்தடத்தை மாற்றுங்கள்"), TEXT("修改路线")},
        {TEXT("TXT_BTN_ReviewEditTicket"), TEXT("Ubah tiket"), TEXT("சீட்டை மாற்றுங்கள்"), TEXT("修改车票")},
        {TEXT("TXT_BTN_ReviewProceed"), TEXT("Pilih bayaran →"), TEXT("கட்டண முறையைத் தேர்ந்தெடுங்கள் →"), TEXT("选择付款方式 →")},
        {TEXT("TXT_ReviewDetails"), TEXT("Semak laluan, tiket, tempoh sah dan jumlah di sebelah kanan."), TEXT("வழித்தடம், சீட்டு, செல்லுபடிக் காலம், மொத்தத்தை வலப்புறம் பாருங்கள்."), TEXT("在右侧核对路线、车票、有效期和总额。")},
        {TEXT("TXT_P5Kick"), TEXT("BAYARAN / 3 DARIPADA 4"), TEXT("கட்டணம் / 3 / 4"), TEXT("付款 / 第 3 步，共 4 步")},
        {TEXT("TXT_P5Title"), TEXT("Bagaimana anda mahu membayar?"), TEXT("எப்படி பணம் செலுத்த விரும்புகிறீர்கள்?"), TEXT("您想怎样付款？")},
        {TEXT("TXT_P5Sub"), TEXT("Pilih kad, telefon atau tunai."), TEXT("அட்டை, கைப்பேசி அல்லது ரொக்கத்தைத் தேர்ந்தெடுங்கள்."), TEXT("选择银行卡、手机或现金。")},
        {TEXT("TXT_P5PayNote"), TEXT("Bayaran bermula apabila anda mengesahkan transaksi."), TEXT("நீங்கள் உறுதிசெய்த பிறகே கட்டணம் தொடங்கும்."), TEXT("确认交易后才会开始付款。")},
        {TEXT("TXT_BTN_PayCard"), TEXT("Sentuh kad atau telefon"), TEXT("அட்டை அல்லது கைப்பேசியைத் தட்டுங்கள்"), TEXT("刷卡或手机感应付款")},
        {TEXT("TXT_BTN_PayCash"), TEXT("Bayar dengan tunai"), TEXT("ரொக்கமாகச் செலுத்துங்கள்"), TEXT("现金付款")},
        {TEXT("TXT_BTN_PayDeclineTest"), TEXT("Simulasi kad ditolak"), TEXT("அட்டை நிராகரிப்பைச் சோதிக்கவும்"), TEXT("模拟银行卡付款被拒")},
        {TEXT("TXT_P6Kick"), TEXT("BAYARAN"), TEXT("கட்டணம்"), TEXT("付款")},
        {TEXT("TXT_P6Sub"), TEXT("Transaksi masih belum selesai."), TEXT("பரிவர்த்தனை இன்னும் முடிவடையவில்லை."), TEXT("交易尚未完成。")},
        {TEXT("TXT_BTN_ProcCancel"), TEXT("Batalkan bayaran"), TEXT("கட்டணத்தை ரத்துசெய்யுங்கள்"), TEXT("取消付款")},
        {TEXT("TXT_BTN_ProcComplete"), TEXT("Simulasi bayaran berjaya"), TEXT("வெற்றிகரமான கட்டணத்தைச் சோதிக்கவும்"), TEXT("模拟付款成功")},
        {TEXT("TXT_P7Appr"), TEXT("✓ BAYARAN DILULUSKAN"), TEXT("✓ கட்டணம் ஏற்கப்பட்டது"), TEXT("✓ 付款成功")},
        {TEXT("TXT_P7Collect"), TEXT("Sila ambil tiket anda"), TEXT("உங்கள் பயணச்சீட்டை எடுத்துக்கொள்ளுங்கள்"), TEXT("请取走车票")},
        {TEXT("TXT_P7Kick"), TEXT("SELESAI / 4 DARIPADA 4"), TEXT("முடிந்தது / 4 / 4"), TEXT("完成 / 第 4 步，共 4 步")},
        {TEXT("TXT_BTN_DoneAnother"), TEXT("Mulakan perjalanan lain"), TEXT("மற்றொரு பயணத்தைத் தொடங்குங்கள்"), TEXT("开始另一段旅程")},
        {TEXT("TXT_BTN_DoneHome"), TEXT("Selesai"), TEXT("முடிக்கவும்"), TEXT("完成")},
        {TEXT("TXT_P8Kick"), TEXT("TAMBAH NILAI KAD"), TEXT("அட்டைக்கு மதிப்பு சேர்த்தல்"), TEXT("交通卡充值")},
        {TEXT("TXT_P8Title"), TEXT("Berapa banyak mahu ditambah?"), TEXT("எவ்வளவு மதிப்பு சேர்க்க விரும்புகிறீர்கள்?"), TEXT("您要充值多少？")},
        {TEXT("TXT_P8Sub"), TEXT("Pilih amaun untuk ditambah pada kad."), TEXT("அட்டைக்குச் சேர்க்கும் தொகையைத் தேர்ந்தெடுங்கள்."), TEXT("请选择充值金额。")},
        {TEXT("TXT_BTN_TU10"), TEXT("Tambah S$10"), TEXT("S$10 சேர்க்கவும்"), TEXT("充值 S$10")},
        {TEXT("TXT_BTN_TU20"), TEXT("Tambah S$20"), TEXT("S$20 சேர்க்கவும்"), TEXT("充值 S$20")},
        {TEXT("TXT_BTN_TU50"), TEXT("Tambah S$50"), TEXT("S$50 சேர்க்கவும்"), TEXT("充值 S$50")},
        {TEXT("TXT_BTN_TUConfirm"), TEXT("Semak tambah nilai →"), TEXT("சேர்த்த தொகையைச் சரிபாருங்கள் →"), TEXT("核对充值 →")},
        {TEXT("TXT_P9Kick"), TEXT("BANTUAN"), TEXT("உதவி"), TEXT("帮助")},
        {TEXT("TXT_P9Title"), TEXT("Bagaimana kami boleh membantu?"), TEXT("நாங்கள் எப்படி உதவலாம்?"), TEXT("您需要什么帮助？")},
        {TEXT("TXT_P9Sub"), TEXT("Kakitangan boleh membantu anda sebelum pembelian."), TEXT("சீட்டு வாங்குமுன் ஊழியர் உதவி பெறலாம்."), TEXT("购票前也可以向工作人员求助。")},
        {TEXT("TXT_BTN_HelpPerson"), TEXT("Panggil kakitangan\nKakitangan akan datang ke kiosk ini."), TEXT("ஊழியரை அழையுங்கள்\nஊழியர் இந்த இயந்திரத்திடம் வருவார்."), TEXT("呼叫工作人员\n工作人员会来到这台售票机。")},
        {TEXT("TXT_BTN_HelpVoice"), TEXT("Bercakap dengan kakitangan"), TEXT("ஊழியருடன் பேசுங்கள்"), TEXT("与工作人员通话")},
        {TEXT("TXT_P10Kick"), TEXT("NOTIS SISTEM"), TEXT("அமைப்பு அறிவிப்பு"), TEXT("系统提示")},
        {TEXT("TXT_BTN_RecAction1"), TEXT("Pilih pilihan lain"), TEXT("வேறு விருப்பத்தைத் தேர்ந்தெடுங்கள்"), TEXT("选择其他选项")},
        {TEXT("TXT_BTN_RecAction2"), TEXT("Minta bantuan"), TEXT("உதவி கேளுங்கள்"), TEXT("寻求帮助")},
    };

    const FTranslation Phrases[] = {
        {TEXT("1  Destination"), TEXT("1  Destinasi"), TEXT("1  செல்லும் இடம்"), TEXT("1  目的地")},
        {TEXT("2  Ticket"), TEXT("2  Tiket"), TEXT("2  பயணச்சீட்டு"), TEXT("2  车票")},
        {TEXT("3  Review & pay"), TEXT("3  Semak dan bayar"), TEXT("3  சரிபார்த்துச் செலுத்துங்கள்"), TEXT("3  核对并付款")},
        {TEXT("4  Collect"), TEXT("4  Ambil tiket"), TEXT("4  சீட்டை எடுத்துக்கொள்ளுங்கள்"), TEXT("4  取票")},
        {TEXT("← Back to start"), TEXT("← Kembali ke mula"), TEXT("← தொடக்கத்திற்குத் திரும்புங்கள்"), TEXT("← 返回首页")},
        {TEXT("← Back"), TEXT("← Kembali"), TEXT("← பின்செல்"), TEXT("← 返回")},
        {TEXT("YOUR LIVE TICKET"), TEXT("TIKET ANDA"), TEXT("உங்கள் பயணச்சீட்டு"), TEXT("您的车票")},
        {TEXT("YOUR CARD SUMMARY"), TEXT("RINGKASAN KAD"), TEXT("அட்டை விவரம்"), TEXT("交通卡摘要")},
        {TEXT("GOING TO:"), TEXT("DESTINASI:"), TEXT("செல்லும் இடம்:"), TEXT("目的地：")},
        {TEXT("START:"), TEXT("MULA:"), TEXT("தொடக்கம்:"), TEXT("起点：")},
        {TEXT("MY TICKET:"), TEXT("TIKET:"), TEXT("சீட்டு:"), TEXT("车票：")},
        {TEXT("TOTAL:"), TEXT("JUMLAH:"), TEXT("மொத்தம்:"), TEXT("总额：")},
        {TEXT("VALIDITY:"), TEXT("TEMPOH SAH:"), TEXT("செல்லுபடிக் காலம்:"), TEXT("有效期：")},
        {TEXT("Not chosen yet"), TEXT("Belum dipilih"), TEXT("இன்னும் தேர்வுசெய்யப்படவில்லை"), TEXT("尚未选择")},
        {TEXT("Shown before payment"), TEXT("Dipaparkan sebelum bayaran"), TEXT("கட்டணத்திற்கு முன் காட்டப்படும்"), TEXT("付款前显示")},
        {TEXT("No payment is taken until you approve."), TEXT("Bayaran hanya diambil selepas anda mengesahkan."), TEXT("உறுதிசெய்த பின்னரே கட்டணம் வசூலிக்கப்படும்."), TEXT("您确认后才会扣款。")},
        {TEXT("WAITING FOR CONTACTLESS TAP"), TEXT("MENUNGGU SENTUHAN KAD"), TEXT("அட்டையைத் தட்டக் காத்திருக்கிறது"), TEXT("等待感应刷卡")},
        {TEXT("WAITING FOR CASH INSERTION"), TEXT("MENUNGGU WANG TUNAI"), TEXT("ரொக்கம் செலுத்தக் காத்திருக்கிறது"), TEXT("等待投入现金")},
        {TEXT("Insert accepted notes or coins into the lower-left slot."), TEXT("Masukkan wang kertas atau syiling ke slot kiri bawah."), TEXT("கீழ் இடப்புறத் துளையில் தாள்கள் அல்லது நாணயங்களைச் செலுத்துங்கள்."), TEXT("请将纸币或硬币投入左下方的投币口。")},
        {TEXT("Tap your card or phone"), TEXT("Sentuh kad atau telefon"), TEXT("அட்டை அல்லது கைப்பேசியைத் தட்டுங்கள்"), TEXT("请感应银行卡或手机")},
        {TEXT("Keep your card or phone at the reader until confirmation."), TEXT("Biarkan kad atau telefon pada pembaca sehingga disahkan."), TEXT("உறுதிப்படுத்தும் வரை அட்டை அல்லது கைப்பேசியை வாசிப்பானில் வையுங்கள்."), TEXT("请将银行卡或手机靠近读卡器，直至确认。")},
        {TEXT("Payment declined"), TEXT("Bayaran ditolak"), TEXT("கட்டணம் நிராகரிக்கப்பட்டது"), TEXT("付款被拒")},
        {TEXT("Route temporarily unavailable"), TEXT("Laluan tidak tersedia buat sementara"), TEXT("வழித்தடம் தற்காலிகமாகக் கிடைக்கவில்லை"), TEXT("路线暂不可用")},
        {TEXT("Jurong East / NS1 service is currently paused for maintenance. Please choose another destination or ask staff for help."), TEXT("Perkhidmatan Jurong East / NS1 dihentikan sementara untuk penyelenggaraan. Pilih destinasi lain atau minta bantuan."), TEXT("ஜூரோங் ஈஸ்ட் / NS1 சேவை பராமரிப்புக்காக நிறுத்தப்பட்டுள்ளது. வேறு இடத்தைத் தேர்ந்தெடுங்கள் அல்லது உதவி கேளுங்கள்."), TEXT("裕廊东 / NS1 因维护暂时停运。请选择其他目的地或寻求帮助。")},
        {TEXT("Your card could not be authorised. No charge was made. Try another card, pay with cash, or ask staff for help."), TEXT("Kad ditolak. Tiada caj dikenakan. Cuba kad lain, tunai atau minta bantuan."), TEXT("அட்டை ஏற்கப்படவில்லை. கட்டணம் வசூலிக்கப்படவில்லை. வேறு அட்டை, ரொக்கம் அல்லது உதவியை முயலுங்கள்."), TEXT("银行卡未获授权，未扣款。请更换银行卡、使用现金或求助。")},
        {TEXT("Payment is complete. Take your printed ticket from the tray below."), TEXT("Bayaran selesai. Ambil tiket bercetak dari dulang di bawah."), TEXT("கட்டணம் முடிந்தது. கீழுள்ள தட்டிலிருந்து சீட்டை எடுத்துக்கொள்ளுங்கள்."), TEXT("付款完成。请从下方取票口取走车票。")},
        {TEXT("You're ready to go."), TEXT("Anda sedia untuk berlepas."), TEXT("நீங்கள் பயணிக்கத் தயார்."), TEXT("您可以出发了。")},
        {TEXT("Current balance"), TEXT("Baki semasa"), TEXT("தற்போதைய இருப்பு"), TEXT("当前余额")},
        {TEXT("New Balance:"), TEXT("Baki baharu:"), TEXT("புதிய இருப்பு:"), TEXT("新余额：")},
        {TEXT("Your card balance has been updated. You may now remove your card."), TEXT("Baki kad telah dikemas kini. Anda boleh mengambil kad."), TEXT("அட்டை இருப்பு புதுப்பிக்கப்பட்டது. அட்டையை எடுத்துக்கொள்ளலாம்."), TEXT("交通卡余额已更新。您可以取走卡片。")},
        {TEXT("Valid Today"), TEXT("Sah hari ini"), TEXT("இன்று செல்லுபடியாகும்"), TEXT("当天有效")},
        {TEXT("DIRECT ROUTE"), TEXT("LALUAN TERUS"), TEXT("நேரடி வழித்தடம்"), TEXT("直达")},
        {TEXT("1 TRANSFER"), TEXT("1 PERTUKARAN"), TEXT("1 இடமாற்றம்"), TEXT("换乘 1 次")},
        {TEXT(" STOPS"), TEXT(" HENTIAN"), TEXT(" நிறுத்தங்கள்"), TEXT(" 站")},
        {TEXT("APPROX."), TEXT("ANGGARAN"), TEXT("சுமார்"), TEXT("约")},
        {TEXT(" MIN"), TEXT(" MIN"), TEXT(" நிமிடம்"), TEXT(" 分钟")},
        {TEXT("In-person staff requested — a team member is on the way. Your purchase remains paused."), TEXT("Bantuan diminta. Kakitangan dalam perjalanan. Pembelian anda dijeda."), TEXT("உதவி கோரப்பட்டது. ஊழியர் வருகிறார். உங்கள் கொள்முதல் இடைநிறுத்தப்பட்டுள்ளது."), TEXT("已请求协助，工作人员正在赶来。您的购票流程已暂停。")},
        {TEXT("Voice call requested — connecting to station staff. Your purchase remains paused."), TEXT("Panggilan diminta. Sedang menghubungi kakitangan. Pembelian anda dijeda."), TEXT("அழைப்பு கோரப்பட்டது. ஊழியருடன் இணைக்கப்படுகிறது. உங்கள் கொள்முதல் இடைநிறுத்தப்பட்டுள்ளது."), TEXT("正在联系车站工作人员。您的购票流程已暂停。")},
        {TEXT("PRIVACY & CONTROL: Ticket details are shared with staff only if the user chooses."), TEXT("BUTIRAN TIKET: Dikongsi dengan kakitangan hanya dengan izin anda."), TEXT("உங்கள் அனுமதியுடன் மட்டுமே சீட்டு விவரங்கள் ஊழியருடன் பகிரப்படும்."), TEXT("只有您同意后，车票信息才会与工作人员共享。")},
        {TEXT("TRANSIT CARD"), TEXT("KAD TRANSIT"), TEXT("போக்குவரத்து அட்டை"), TEXT("交通卡")},
    };
}

FString TranslateKioskText(FName WidgetName, const FString& English, ENexoKioskLanguage Language)
{
    if (Language == ENexoKioskLanguage::English)
    {
        return English;
    }
    for (const FTranslation& Translation : Labels)
    {
        if (WidgetName == FName(Translation.Key))
        {
            return Translation.Get(Language);
        }
    }

    FString Output = English;
    for (const FTranslation& Translation : Phrases)
    {
        Output.ReplaceInline(Translation.Key, Translation.Get(Language), ESearchCase::IgnoreCase);
    }
    return Output;
}

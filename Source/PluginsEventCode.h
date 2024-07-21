#ifndef AML_PAGES_PLUGIN_ENUM_CODE_EVENT_HEADER_STORAGE
	#define AML_PAGES_PLUGIN_ENUM_CODE_EVENT_HEADER_STORAGE
	enum AML_EVENTCODE {		
		 AEC_DOCOPEN		=1
		
		,AEC_FIRST			=AEC_DOCOPEN
		
		,AEC_DOCCLOSE		=2
		,AEC_EXITAMLPAGES	=3
		,AEC_DOCACTIVATE	=4	//����������� ���������
		,AEC_PAGEOPEN		=5	//�������� �������
		,AEC_PAGESAVE		=6	//�������� �������� � ���������
		,AEC_PAGEDELETE		=7	//c������� �������
		,AEC_PAGEADDED		=8	//c������� ���������
		,AEC_EDITOR			=10
		,AEC_STARTAMLPAGES	=11	//Start of Aml Pages (main window is created)
		,AEC_CLOSEMAINWND	=12	//Close Main Window of Aml Pages
		,AEC_LOGMESSAGE		=13	//��������� � ���
		,AEC_DOCSAVE		=14	//���������� ���������
		
		,AEC_LAST			= AEC_DOCSAVE//AEC_CLOSEMAINWND
	};
#endif//AML_PAGES_PLUGIN_ENUM_CODE_EVENT_HEADER_STORAGE
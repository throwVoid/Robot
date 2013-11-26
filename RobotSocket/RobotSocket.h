


class Socket
{
public:
	
	Socket(ISocketManage&);

	virtual ~Socket(){};

	ISocketManage& Manage() const;

	/**事件回调*/
	//TCP监听服务器ACCEPT
	void OnAccept()

protected:
private:
	ISocketManage& socketManage;
};

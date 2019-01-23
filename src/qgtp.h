#ifndef QGTP_H
#define QGTP_H

#include <QProcess>

#include "goboard.h"
#include "textview.h"

class Engine
{
	QString m_title;
	QString m_path;
	QString m_args;
	QString m_komi;
	bool m_analysis;

public:
	Engine (const QString &title, const QString &path, const QString &args, const QString &komi, bool analysis)
		: m_title (title), m_path (path), m_args (args), m_komi (komi), m_analysis (analysis)
	{
	}
	QString title() const { return m_title; };
	QString path() const { return m_path; };
	QString args() const { return m_args; };
	QString komi() const { return m_komi; }
	bool analysis () const { return m_analysis; }

	int operator== (Engine h)
		{ return (this->m_title == h.m_title); };
	bool operator< (Engine h)
		{ return (this->m_title < h.m_title); };
};


class GTP_Process;

class Gtp_Controller
{
	friend class GTP_Process;
	QWidget *m_parent;

public:
	Gtp_Controller (QWidget *p) : m_parent (p) { }
	GTP_Process *create_gtp (const Engine &engine, int size, double komi, int hc);
	virtual void gtp_played_move (int x, int y) = 0;
	virtual void gtp_played_pass () = 0;
	virtual void gtp_played_resign () = 0;
	virtual void gtp_startup_success () = 0;
	virtual void gtp_exited () = 0;
	virtual void gtp_failure (const QString &) = 0;
	virtual void gtp_eval (const QString &)
	{
	}
};

class GTP_Process : public QProcess
{
	Q_OBJECT

	QString m_buffer;

	TextView m_dlg;
	Gtp_Controller *m_controller;

	int m_size;
	double m_komi;
	int m_hc;
	bool m_started = false;
	bool m_stopped = false;

	typedef void (GTP_Process::*t_receiver) (const QString &);
	QMap <int, t_receiver> m_receivers;

	/* Number of the next request.  */
	int req_cnt;
	void send_request(const QString &, t_receiver = nullptr);

	void startup_part2 (const QString &);
	void startup_part3 (const QString &);
	void startup_part4 (const QString &);
	void startup_part5 (const QString &);
	void startup_part6 (const QString &);
	void receive_move (const QString &);
	void internal_quit ();

public slots:
	void slot_started ();
	void slot_finished (int exitcode, QProcess::ExitStatus status);
	void slot_error (QProcess::ProcessError);
	void slot_receive_stdout ();
	void slot_startup_messages ();
	void slot_abort_request (bool);

public:
	GTP_Process (QWidget *parent, Gtp_Controller *c, const Engine &engine,
		     int size, float komi, int hc);
	~GTP_Process ();
	bool started () { return m_started; }
	bool stopped () { return m_stopped; }

	void clear_board () { send_request ("clear_board"); }
	void request_move (stone_color col);
	void played_move (stone_color col, int x, int y);
	void played_move_pass (stone_color col);
	void played_move_resign (stone_color col);
	void analyze (stone_color col, int interval);
	void pause_analysis ();

	void quit ();
};

#endif

drop table if exists public.Contactlist;
drop table if exists public.Chatlog;
drop table if exists public.User;

create table public.User (
	id serial primary key,
	username varchar(64) not null unique,
	public_key bytea not null,
	password_hash bytea not null
);

create table public.Chatlog (
	id serial primary key,
	user_id int references public.User(id) not null,
	time_stamp timestamp not null,
	encrypted_content bytea not null,
	unique(user_id, time_stamp)
);

create table public.Contactlist (
	id serial primary key,
	user_id int references public.User(id) not null unique,
	encrypted_content bytea not null,
	signature bytea not null
); 

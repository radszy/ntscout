package main

import (
	"archive/zip"
	"errors"
	"flag"
	"fmt"
	"io"
	"io/ioutil"
	"log"
	"os"
	"path/filepath"
	"strings"
)

const APP_VERSION = "0.1"

var appver *bool = flag.Bool("v", false, "Print the version number.")
var ntsver *float64 = flag.Float64("n", 0.0, "Version of NTScout")
var user *string = flag.String("u", "", "Username used to login.")
var pass *string = flag.String("p", "", "Password used to login.")

func main() {
	flag.Parse()

	if *appver {
		fmt.Println("Version:", APP_VERSION)
		return
	}

	if *user == "" || *pass == "" || *ntsver == 0.0 {
		fmt.Println("You need to specify username, password and version of NTScout.")
		flag.PrintDefaults()
		return
	}

	fmt.Print("Updating binary files... ")
	err := updateBinary()
	if err != nil {
		fmt.Println(err)
	} else {
		fmt.Println("done.")
	}

	fmt.Print("Updating country data... ")
	err = updateCountry()
	if err != nil {
		fmt.Println(err)
	} else {
		fmt.Println("done.")
	}

}

func updateCountry() error {
	if ret, err := login(user, pass); !ret {
		return err
	}

	countries, err := countries()
	if err != nil {
		return err
	}

	name, err := namesEn()
	if err != nil {
		return err
	}

	length := len(countries.Country)
	for i := 0; i < length; i++ {
		countries.Country[i].NameEn = name[i]
	}

	file, err := os.OpenFile("country.dat", os.O_WRONLY|os.O_TRUNC, 0777)
	if err != nil {
		return err
	}
	defer file.Close()

	for _, c := range countries.Country {
		file.WriteString(c.ID + "," + c.Name + "," + c.NameEn + "," +
			c.Divisions + "," + c.Users + "\n")
	}

	return nil
}

func updateBinary() error {
	url, err := releases()
	if err != nil {
		return err
	}

	if url == "" {
		return errors.New("Nothing to update.")
	}

	err = download(url, os.TempDir())
	if err != nil {
		return err
	}

	pwd, err := filepath.Abs(filepath.Dir(os.Args[0]))
	tmp := os.TempDir() + "/NTScout"

	err = unzip(tmp+".zip", os.TempDir())
	if err != nil {
		return err
	}

	err = removeOldStuff(pwd)
	if err != nil {
		fmt.Println(err)
	}

	err = moveNewStuff(pwd, tmp)
	if err != nil {
		fmt.Println(err)
	}

	err = clean(tmp)
	if err != nil {
		return err
	}

	return nil
}

func removeOldStuff(src string) error {
	entries, err := ioutil.ReadDir(src)
	if err != nil {
		return err
	}

	for _, entry := range entries {
		if entry.Name() == "Updater" {
			continue
		}
		err = os.RemoveAll(src + "/" + entry.Name())
		if err != nil {
			return err
		}
	}

	return nil
}

func moveNewStuff(dst, src string) error {
	entries, err := ioutil.ReadDir(src)
	if err != nil {
		return err
	}

	for _, entry := range entries {
		if entry.Name() == "Updater" {
			err = os.Rename(src+"/"+entry.Name(), dst+"/"+entry.Name()+"-new")
			if err != nil {
				return err
			}
			continue
		}
		err = os.Rename(src+"/"+entry.Name(), dst+"/"+entry.Name())
		if err != nil {
			return err
		}
	}

	return nil
}

func clean(dir string) error {
	err := os.RemoveAll(dir)
	if err != nil {
		return err
	}

	err = os.Remove(dir + ".zip")
	if err != nil {
		return err
	}
	return nil
}

func unzip(src, dest string) error {
	r, err := zip.OpenReader(src)
	if err != nil {
		return err
	}
	defer r.Close()

	for _, f := range r.File {
		rc, err := f.Open()
		if err != nil {
			return err
		}
		defer rc.Close()

		fpath := filepath.Join(dest, f.Name)
		if f.FileInfo().IsDir() {
			os.MkdirAll(fpath, f.Mode())
		} else {
			var fdir string
			if lastIndex := strings.LastIndex(fpath, string(os.PathSeparator)); lastIndex > -1 {
				fdir = fpath[:lastIndex]
			}

			err = os.MkdirAll(fdir, f.Mode())
			if err != nil {
				log.Fatal(err)
				return err
			}
			f, err := os.OpenFile(
				fpath, os.O_WRONLY|os.O_CREATE|os.O_TRUNC, f.Mode())
			if err != nil {
				return err
			}
			defer f.Close()

			_, err = io.Copy(f, rc)
			if err != nil {
				return err
			}
		}
	}
	return nil
}
